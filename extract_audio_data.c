#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Estructura simplificada del header WAV (ELIMINAR AL FINAL)
typedef struct {
    // RIFF Header
    char riff[4];           // "RIFF"
    uint32_t file_size;     // Tamaño del archivo - 8
    char wave[4];           // "WAVE"
    
    // fmt subchunk
    char fmt[4];            // "fmt "
    uint32_t fmt_size;      // 16 para PCM
    uint16_t audio_format;  // 1 = PCM
    uint16_t num_channels;  // 1 = Mono, 2 = Stereo
    uint32_t sample_rate;   // 22050, 44100, etc.
    uint32_t byte_rate;     // sample_rate * num_channels * bits_per_sample/8
    uint16_t block_align;   // num_channels * bits_per_sample/8
    uint16_t bits_per_sample; // 16
    
    // data subchunk
    char data[4];           // "data"
    uint32_t data_size;     // Tamaño de los datos de audio
} __attribute__((packed)) WAV_Header;

// Estructura para información del archivo (MANTENER)
typedef struct { 
    uint32_t sample_rate;
    uint16_t num_channels;
    uint16_t bits_per_sample;
    uint32_t data_size;         // Tamaño de los datos de audio en bytes
    uint32_t num_samples;       // Número total de muestras
    uint32_t duration_seconds;  // Duración en segundos
} AudioInfo;


// ===================================================================================================================================


// Función para leer el header WAV y encontrar el chunk de datos (MANTENER)
int leer_wav_info(const char *filepath, AudioInfo *info, long *data_offset) {
    FILE *f = fopen(filepath, "rb");
    if (!f) {
        printf("Error: No se pudo abrir el archivo %s\n", filepath);
        return -1;
    }

    char chunk_id[5] = {0};
    uint32_t chunk_size;
    
    // Leer header RIFF
    fread(chunk_id, 1, 4, f);
    if (strncmp(chunk_id, "RIFF", 4) != 0) {
        printf("Error: No es un archivo RIFF válido\n");
        fclose(f);
        return -1;
    }
    
    fseek(f, 4, SEEK_CUR); // Skip size
    fread(chunk_id, 1, 4, f);
    if (strncmp(chunk_id, "WAVE", 4) != 0) {
        printf("Error: No es un archivo WAVE válido\n");
        fclose(f);
        return -1;
    }

    // Buscar chunks fmt y data
    int found_fmt = 0, found_data = 0;
    
    while (fread(chunk_id, 1, 4, f) == 4) {
        fread(&chunk_size, 4, 1, f);
        chunk_id[4] = '\0';
        long next_chunk = ftell(f) + chunk_size;

        if (strcmp(chunk_id, "fmt ") == 0) {
            uint16_t audio_format;
            fread(&audio_format, 2, 1, f);
            fread(&info->num_channels, 2, 1, f);
            fread(&info->sample_rate, 4, 1, f);
            fseek(f, 6, SEEK_CUR);
            fread(&info->bits_per_sample, 2, 1, f);
            found_fmt = 1;
            fseek(f, next_chunk, SEEK_SET);
        } else if (strcmp(chunk_id, "data") == 0) {
            info->data_size = chunk_size;
            *data_offset = ftell(f);  // Guardar posición donde empiezan los datos
            found_data = 1;
            break;  // Ya encontramos todo lo que necesitamos
        } else {
            fseek(f, next_chunk, SEEK_SET);
        }
    }

    fclose(f);

    if (!found_fmt || !found_data) {
        printf("Error: No se encontraron los chunks necesarios\n");
        return -1;
    }

    // Calcular información derivada
    uint32_t bytes_per_sample = info->bits_per_sample / 8;
    info->num_samples = info->data_size / (info->num_channels * bytes_per_sample);
    info->duration_seconds = info->num_samples / info->sample_rate;

    return 0;
}


// Función para extraer datos de audio en bloques (útil para streaming) (MANTENER)
int extraer_audio_bloques(const char *filepath, long data_offset, uint32_t data_size, void (*callback)(int16_t *buffer, uint32_t num_samples, void *user_data), void *user_data, uint32_t block_size) {
    FILE *f = fopen(filepath, "rb");
    if (!f) return -1;

    // Posicionarse al inicio de los datos de audio
    fseek(f, data_offset, SEEK_SET);

    // Buffer para leer bloques
    int16_t *buffer = (int16_t*)malloc(block_size * sizeof(int16_t));
    if (!buffer) {
        fclose(f);
        return -1;
    }

    uint32_t bytes_read;
    uint32_t total_read = 0;
    
    while (total_read < data_size) {
        uint32_t bytes_to_read = block_size * sizeof(int16_t);
        if (total_read + bytes_to_read > data_size) {
            bytes_to_read = data_size - total_read;
        }

        bytes_read = fread(buffer, 1, bytes_to_read, f);
        if (bytes_read == 0) break;

        uint32_t samples_in_block = bytes_read / sizeof(int16_t);
        
        // Llamar al callback con el bloque de datos
        if (callback) {
            callback(buffer, samples_in_block, user_data);
        }

        total_read += bytes_read;
    }

    free(buffer);
    fclose(f);
    return 0;
}


// Callback para escribir en memoria (simulación) (MANTENER)
void escribir_a_memoria(int16_t *buffer, uint32_t num_samples, void *user_data) {
    // En este ejemplo simulamos la escritura a memoria
    // En tu caso real, aquí harías la transferencia DMA o escritura directa
    uint32_t *bytes_written = (uint32_t*)user_data;
    *bytes_written += num_samples * sizeof(int16_t);
    
    // Aquí irían las instrucciones para escribir a una dirección de memoria específica
    // Por ejemplo:
    // volatile int16_t *memoria_destino = (volatile int16_t*)0x30000000; // Dirección base
    // memcpy(memoria_destino + offset, buffer, num_samples * sizeof(int16_t));
}


// ===================================================================================================================================


// Callback para guardar en archivo WAV (ELIMINAR AL FINAL)
void guardar_en_archivo(int16_t *buffer, uint32_t num_samples, void *user_data) {
    FILE *f_out = (FILE*)user_data;
    if (f_out) {
        fwrite(buffer, sizeof(int16_t), num_samples, f_out);
    }
}


// PARA VALIDACION DE LA EXTRACCION DE AUDIO (ELIMINAR AL FINAL)
void crear_wav_header(WAV_Header *header, uint32_t sample_rate, uint16_t num_channels, uint16_t bits_per_sample, uint32_t data_size) {
    memcpy(header->riff, "RIFF", 4);
    header->file_size = data_size + sizeof(WAV_Header) - 8;
    memcpy(header->wave, "WAVE", 4);
    
    memcpy(header->fmt, "fmt ", 4);
    header->fmt_size = 16;
    header->audio_format = 1;  // PCM
    header->num_channels = num_channels;
    header->sample_rate = sample_rate;
    header->bits_per_sample = bits_per_sample;
    header->byte_rate = sample_rate * num_channels * bits_per_sample / 8;
    header->block_align = num_channels * bits_per_sample / 8;
    
    memcpy(header->data, "data", 4);
    header->data_size = data_size;
}


// PARA LA VALIDACION DE LA EXTRACCION DE AUDIO (ELIMINAR AL FINAL)
int guardar_audio_extraido(const char *input_file, long data_offset, AudioInfo *info, const char *output_file) {
    // Abrir archivo de salida
    FILE *f_out = fopen(output_file, "wb");
    if (!f_out) {
        printf("Error: No se pudo crear el archivo de salida\n");
        return -1;
    }
    
    // Crear y escribir header
    WAV_Header header;
    crear_wav_header(&header, info->sample_rate, info->num_channels, 
                    info->bits_per_sample, info->data_size);
    fwrite(&header, sizeof(WAV_Header), 1, f_out);
    
    // Extraer y escribir los datos de audio
    extraer_audio_bloques(input_file, data_offset, info->data_size, 
                         guardar_en_archivo, f_out, 1024);
    
    fclose(f_out);
    return 0;
}


// ===================================================================================================================================


int main(int argc, char *argv[]) {
    const char *filepath;
    
    if (argc > 1) {
        filepath = argv[1];
    } else {
        filepath = "../../media/sd/music/PRESIDENT - In the Name of the Father_embebido.wav";
    }

    AudioInfo info;
    long data_offset;

    // 1. Leer información del archivo
    if (leer_wav_info(filepath, &info, &data_offset) != 0) {
        return 1;
    }

    // 2. Extraer y guardar audio
    uint32_t block_size = 1024;  // Tamaño del bloque en muestras
    const char *output_file = "extracted_audio.wav";
    
    if (guardar_audio_extraido(filepath, data_offset, &info, output_file) == 0) {
        printf("Extracción completada: %s\n", output_file);
    }

    return 0;
}
