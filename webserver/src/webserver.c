/*
 * Audio Player Web Server
 * Proyecto 2 - Sistemas Empotrados
 * 
 * Servidor web para mostrar información del reproductor de audio
 * Diseñado para correr en DE1-SOC ARM Cortex-A9
 */

#include "mongoose.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Configuración del servidor
#define HTTP_PORT "8080"
#define POLL_PERIOD_MS 1000

// Variables globales para información de la canción
typedef struct {
    char song_name[128];
    char artist[128];
    char album[128];
    int current_time_sec;    // Tiempo actual en segundos
    int total_time_sec;      // Duración total en segundos
    int is_playing;          // 0 = pausado, 1 = reproduciendo
    int current_track;       // Número de pista actual
    int total_tracks;        // Total de pistas
} AudioInfo;

static AudioInfo audio_info = {
    .song_name = "No Song Loaded",
    .artist = "Unknown Artist",
    .album = "Unknown Album",
    .current_time_sec = 0,
    .total_time_sec = 180,
    .is_playing = 0,
    .current_track = 1,
    .total_tracks = 5
};

static int s_signo = 0;

// Manejador de señales para salida limpia
static void signal_handler(int signo) {
    s_signo = signo;
}

// Función para formatear tiempo en MM:SS
static void format_time(int seconds, char *buffer, size_t size) {
    int minutes = seconds / 60;
    int secs = seconds % 60;
    snprintf(buffer, size, "%02d:%02d", minutes, secs);
}

// Handler para servir el HTML principal
static void serve_homepage(struct mg_connection *c) {
    const char *html = 
        "<!DOCTYPE html>\n"
        "<html lang='es'>\n"
        "<head>\n"
        "    <meta charset='UTF-8'>\n"
        "    <meta name='viewport' content='width=device-width, initial-scale=1.0'>\n"
        "    <title>Audio Player - DE1-SOC</title>\n"
        "    <link rel='stylesheet' href='/style.css'>\n"
        "</head>\n"
        "<body>\n"
        "    <div class='container'>\n"
        "        <div class='player'>\n"
        "            <h1 class='title'>🎵 Audio Player DE1-SOC</h1>\n"
        "            <div class='track-info'>\n"
        "                <h2 id='song'>Cargando...</h2>\n"
        "                <p id='artist'>Artista</p>\n"
        "                <p id='album'>Álbum</p>\n"
        "            </div>\n"
        "            <div class='time-info'>\n"
        "                <span id='current-time'>00:00</span>\n"
        "                <div class='progress-bar'>\n"
        "                    <div id='progress' class='progress'></div>\n"
        "                </div>\n"
        "                <span id='total-time'>00:00</span>\n"
        "            </div>\n"
        "            <div class='status'>\n"
        "                <span id='status'>⏸️ Pausado</span>\n"
        "                <span id='track-number'>Pista 1/5</span>\n"
        "            </div>\n"
        "        </div>\n"
        "        <div class='info'>\n"
        "            <p>Sistema: ARM Cortex-A9 @ DE1-SOC</p>\n"
        "            <p>Puerto: 8080</p>\n"
        "        </div>\n"
        "    </div>\n"
        "    <script src='/app.js'></script>\n"
        "</body>\n"
        "</html>";
    
    mg_http_reply(c, 200, "Content-Type: text/html\r\n", "%s", html);
}

// Handler para el endpoint API que retorna info actual
static void serve_api_status(struct mg_connection *c) {
    char current_time_str[16];
    char total_time_str[16];
    
    format_time(audio_info.current_time_sec, current_time_str, sizeof(current_time_str));
    format_time(audio_info.total_time_sec, total_time_str, sizeof(total_time_str));
    
    int progress = (audio_info.total_time_sec > 0) ? 
                   (audio_info.current_time_sec * 100 / audio_info.total_time_sec) : 0;
    
    mg_http_reply(c, 200, "Content-Type: application/json\r\n",
        "{"
        "\"song\":\"%s\","
        "\"artist\":\"%s\","
        "\"album\":\"%s\","
        "\"current_time\":\"%s\","
        "\"total_time\":\"%s\","
        "\"current_sec\":%d,"
        "\"total_sec\":%d,"
        "\"is_playing\":%d,"
        "\"progress\":%d,"
        "\"track\":%d,"
        "\"total_tracks\":%d"
        "}\n",
        audio_info.song_name,
        audio_info.artist,
        audio_info.album,
        current_time_str,
        total_time_str,
        audio_info.current_time_sec,
        audio_info.total_time_sec,
        audio_info.is_playing,
        progress,
        audio_info.current_track,
        audio_info.total_tracks
    );
}

// Handler principal de HTTP
static void fn(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        
        // Endpoint: GET /
        if (mg_strcmp(hm->uri, mg_str("/")) == 0) {
            serve_homepage(c);
        }
        // Endpoint: GET /api/status
        else if (mg_strcmp(hm->uri, mg_str("/api/status")) == 0) {
            serve_api_status(c);
        }
        // Endpoint: GET /style.css
        else if (mg_strcmp(hm->uri, mg_str("/style.css")) == 0) {
            struct mg_http_serve_opts opts = {.root_dir = "../web"};
            mg_http_serve_file(c, hm, "../web/style.css", &opts);
        }
        // Endpoint: GET /app.js
        else if (mg_strcmp(hm->uri, mg_str("/app.js")) == 0) {
            struct mg_http_serve_opts opts = {.root_dir = "../web"};
            mg_http_serve_file(c, hm, "../web/app.js", &opts);
        }
        // 404 Not Found
        else {
            mg_http_reply(c, 404, "", "Not Found\n");
        }
    }
}

// Función para simular actualización de tiempo (para testing)
// En el sistema real, esto se leerá de la FPGA/memoria compartida
static void update_audio_info(void) {
    if (audio_info.is_playing) {
        audio_info.current_time_sec++;
        
        // Si llegamos al final de la canción
        if (audio_info.current_time_sec >= audio_info.total_time_sec) {
            audio_info.current_time_sec = 0;
            audio_info.is_playing = 0;
        }
    }
}

// Timer callback para actualizar info periódicamente
static void timer_fn(void *arg) {
    update_audio_info();
    
    // TODO: Aquí leerás los datos reales desde la FPGA
    // Por ejemplo, leer memoria compartida o PIOs
    // audio_info.current_time_sec = read_fpga_time();
    // audio_info.is_playing = read_fpga_status();
}

int main(void) {
    struct mg_mgr mgr;
    struct mg_connection *c;
    
    // Configurar manejadores de señales
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Inicializar mongoose
    mg_mgr_init(&mgr);
    
    // Crear servidor HTTP
    c = mg_http_listen(&mgr, "http://0.0.0.0:" HTTP_PORT, fn, NULL);
    if (c == NULL) {
        fprintf(stderr, "Error: No se pudo iniciar el servidor en puerto %s\n", HTTP_PORT);
        return 1;
    }
    
    printf("===========================================\n");
    printf("  Audio Player Web Server - DE1-SOC\n");
    printf("===========================================\n");
    printf("Servidor iniciado en http://0.0.0.0:%s\n", HTTP_PORT);
    printf("Presiona Ctrl+C para detener\n\n");
    printf("Endpoints disponibles:\n");
    printf("  GET /              - Interfaz web principal\n");
    printf("  GET /api/status    - Estado actual (JSON)\n");
    printf("===========================================\n\n");
    
    // Configurar datos de prueba
    strcpy(audio_info.song_name, "Bohemian Rhapsody");
    strcpy(audio_info.artist, "Queen");
    strcpy(audio_info.album, "A Night at the Opera");
    audio_info.is_playing = 1;  // Iniciar reproduciendo para demo
    
    // Configurar timer para actualización periódica
    mg_timer_add(&mgr, 1000, MG_TIMER_REPEAT, timer_fn, NULL);
    
    // Event loop principal
    while (s_signo == 0) {
        mg_mgr_poll(&mgr, POLL_PERIOD_MS);
    }
    
    printf("\nDeteniendo servidor...\n");
    mg_mgr_free(&mgr);
    printf("Servidor detenido correctamente.\n");
    
    return 0;
}
