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

// Estructura para metadatos de canción
typedef struct {
    char filename[128];
    char title[64];
    char artist[64];
    char album[64];
    char year[8];
    char duration[8];
} SongMeta;

#define MAX_SONGS 20
static SongMeta songs[MAX_SONGS];
static int song_count = 0;
static int current_song = 0;

// Estado de reproducción
static int current_time_sec = 0;
static int is_playing = 1;

static int s_signo = 0;

// Parser manual para canciones.json
static void load_songs(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[256];
    SongMeta temp = {0};
    int in_song = 0;
    while (fgets(line, sizeof(line), f)) {
        char *p;
        // Detectar inicio de canción (por filename)
        if ((p = strstr(line, "_embebido.wav"))) {
            if (in_song && song_count < MAX_SONGS) {
                songs[song_count++] = temp;
            }
            memset(&temp, 0, sizeof(temp));
            in_song = 1;
            // Extraer filename
            char *start = strchr(line, '"');
            char *end = strchr(line, '"');
            if (start && (end = strchr(start+1, '"'))) {
                int len = end - start - 1;
                if (len > 0 && len < (int)sizeof(temp.filename)) {
                    strncpy(temp.filename, start+1, len);
                    temp.filename[len] = '\0';
                }
            }
        }
        // Extraer metadatos
        if ((p = strstr(line, "\"title\""))) {
            sscanf(p, "\"title\": \"%63[^\"]", temp.title);
        } else if ((p = strstr(line, "\"artist\""))) {
            sscanf(p, "\"artist\": \"%63[^\"]", temp.artist);
        } else if ((p = strstr(line, "\"album\""))) {
            sscanf(p, "\"album\": \"%63[^\"]", temp.album);
        } else if ((p = strstr(line, "\"year\""))) {
            sscanf(p, "\"year\": \"%7[^\"]", temp.year);
        } else if ((p = strstr(line, "\"duration\""))) {
            sscanf(p, "\"duration\": \"%7[^\"]", temp.duration);
        }
    }
    // Agregar última canción
    if (in_song && song_count < MAX_SONGS) {
        songs[song_count++] = temp;
    }
    fclose(f);
    if (song_count == 0) {
        // Fallback si no se pudo leer
        strcpy(songs[0].title, "No Song Loaded");
        strcpy(songs[0].artist, "Unknown Artist");
        strcpy(songs[0].album, "Unknown Album");
        strcpy(songs[0].year, "----");
        strcpy(songs[0].duration, "00:00");
        song_count = 1;
    }
}

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
        "                <p id='year'>Año:</p>\n"
        "                <p id='duration'>Duración:</p>\n"
        "                <p id='filename'>Archivo:</p>\n"
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
        "            <div class='controls' style='text-align:center; margin-top:20px;'>\n"
        "                <button id='prev-btn' class='interactive'>&lt; Anterior</button>\n"
        "                <button id='next-btn' class='interactive'>Siguiente &gt;</button>\n"
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
    format_time(current_time_sec, current_time_str, sizeof(current_time_str));
    // Usar duración de la canción actual
    int total_sec = 0;
    int min = 0, sec = 0;
    sscanf(songs[current_song].duration, "%d:%d", &min, &sec);
    total_sec = min * 60 + sec;
    int progress = (total_sec > 0) ? (current_time_sec * 100 / total_sec) : 0;
    mg_http_reply(c, 200, "Content-Type: application/json\r\n",
        "{"
        "\"filename\":\"%s\"," 
        "\"title\":\"%s\"," 
        "\"artist\":\"%s\"," 
        "\"album\":\"%s\"," 
        "\"year\":\"%s\"," 
        "\"duration\":\"%s\"," 
        "\"current_time\":\"%s\"," 
        "\"current_sec\":%d," 
        "\"total_sec\":%d," 
        "\"is_playing\":%d," 
        "\"progress\":%d," 
        "\"track\":%d," 
        "\"total_tracks\":%d"
        "}\n",
        songs[current_song].filename,
        songs[current_song].title,
        songs[current_song].artist,
        songs[current_song].album,
        songs[current_song].year,
        songs[current_song].duration,
        current_time_str,
        current_time_sec,
        total_sec,
        is_playing,
        progress,
        current_song+1,
        song_count
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
        // Endpoint: POST /api/next
        else if (mg_strcmp(hm->uri, mg_str("/api/next")) == 0) {
            current_song = (current_song + 1) % song_count;
            current_time_sec = 0;
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{\"ok\":1,\"track\":%d}\n", current_song+1);
        }
        // Endpoint: POST /api/prev
        else if (mg_strcmp(hm->uri, mg_str("/api/prev")) == 0) {
            current_song = (current_song - 1 + song_count) % song_count;
            current_time_sec = 0;
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "{\"ok\":1,\"track\":%d}\n", current_song+1);
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
// Simulación de avance de tiempo de reproducción
static void update_audio_info(void) {
    if (is_playing) {
        current_time_sec++;
        // Obtener duración de la canción actual
        int min = 0, sec = 0;
        int total_sec = 0;
        sscanf(songs[current_song].duration, "%d:%d", &min, &sec);
        total_sec = min * 60 + sec;
        // Si llegamos al final de la canción
        if (current_time_sec >= total_sec) {
            current_time_sec = 0;
            is_playing = 0;
        }
    }
}

// Timer callback para actualizar info periódicamente
static void timer_fn(void *arg) {
    update_audio_info();
    // TODO: Aquí leerás los datos reales desde la FPGA
    // Por ejemplo, leer memoria compartida o PIOs
    // current_time_sec = read_fpga_time();
    // is_playing = read_fpga_status();
}

int main(void) {
    struct mg_mgr mgr;
    struct mg_connection *c;

    // Cargar canciones desde JSON
    load_songs("../canciones.json");

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
    printf("  POST /api/next     - Siguiente canción\n");
    printf("  POST /api/prev     - Canción anterior\n");
    printf("===========================================\n\n");

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
