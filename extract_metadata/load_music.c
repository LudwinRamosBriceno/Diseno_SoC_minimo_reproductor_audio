#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>

// Estructura para almacenar metadatos de una canción
typedef struct {
	char titulo[256];
	char artista[256];
	char album[256];
	char ano[32];
	char duracion[32];  // Formato "MM:SS"
} Metadatos;

// Función para escribir una cadena JSON
void escribir_json_string(FILE *f, const char *str) {
	fputc('"', f);
	for (; *str; ++str) {
		if (*str == '"' || *str == '\\') fputc('\\', f);
		fputc(*str, f);
	}
	fputc('"', f);
}

// Función para escribir metadatos en formato JSON
void escribir_metadatos_json(FILE *f, Metadatos *m) {
	fprintf(f, "    {\n");
	fprintf(f, "      \"title\": "); escribir_json_string(f, m->titulo); fprintf(f, ",\n");
	fprintf(f, "      \"artist\": "); escribir_json_string(f, m->artista); fprintf(f, ",\n");
	fprintf(f, "      \"album\": "); escribir_json_string(f, m->album); fprintf(f, ",\n");
	fprintf(f, "      \"year\": "); escribir_json_string(f, m->ano); fprintf(f, ",\n");
	fprintf(f, "      \"duration\": "); escribir_json_string(f, m->duracion); fprintf(f, "\n");
	fprintf(f, "    }");
}

// Función para extraer metadatos de un archivo WAV
void extraer_metadatos_wav(const char *filepath, Metadatos *meta) {
	
	// Abrir el archivo WAV
	FILE *f = fopen(filepath, "rb");
	if (!f) return;

	// Declaracion de variables para la lectura de chunks
	char chunk_id[5] = {0};
	uint32_t chunk_size;
	char chunk_type[5] = {0};
	meta->titulo[0] = '\0';
	meta->artista[0] = '\0';
	meta->album[0] = '\0';
	meta->ano[0] = '\0';
	meta->duracion[0] = '\0';

	// Variables para calcular la duración
	uint32_t sample_rate = 0;
	uint16_t num_channels = 0;
	uint16_t bits_per_sample = 0;
	uint32_t data_size = 0;

	// Leer chunks del archivo WAV, mientras buscamos metadatos
	fseek(f, 12, SEEK_SET);
	while (fread(chunk_id, 1, 4, f) == 4) {

		/// Determina el tamaño del chunk
		fread(&chunk_size, 4, 1, f);				// Leer tamaño del chunk
		chunk_id[4] = '\0';						 	// Asegurar terminación nula
		long next_chunk = ftell(f) + chunk_size; 	// Posición del siguiente chunk

		
		// Si es un chunk conocido, lo procesa (para el caso de metadatos e info)
		if (strcmp(chunk_id, "fmt ") == 0) {

			// Leer información del formato
			uint16_t audio_format;

			fread(&audio_format, 2, 1, f);			// Lee formato de audio
			fread(&num_channels, 2, 1, f);			// Lee número de canales
			fread(&sample_rate, 4, 1, f);			// Lee sample rate
			fseek(f, 6, SEEK_CUR); 					// Skip de byte rate y block align
			fread(&bits_per_sample, 2, 1, f);		// Lee los bits por muestra
			fseek(f, next_chunk, SEEK_SET);			// Se mueve al siguiente chunk


		// Si es el chunk de datos (para el caso de duración)
		} else if (strcmp(chunk_id, "data") == 0) {

			// Guardar el tamaño del chunk de datos
			data_size = chunk_size;					// Tamaño de datos de audio
			fseek(f, next_chunk, SEEK_SET);			// Se mueve al siguiente chunk
		

		// Si es el chunk de lista (para el caso de metadatos)
		} else if (strcmp(chunk_id, "LIST") == 0) {

			fread(chunk_type, 1, 4, f);				// Leer el tipo de lista/subchunk
			chunk_type[4] = '\0';					// Asegurar terminación nula
			
			// Si es un chunk INFO, procesa los metadatos (para el caso de metadatos)
			if (strcmp(chunk_type, "INFO") == 0) {

				// Mientras haya datos en el chunk INFO
				long info_end = ftell(f) + chunk_size - 4;
				while (ftell(f) < info_end - 8) {
					
					char info_id[5] = {0};			// ID del campo de información
					uint32_t info_size;				// Tamaño del campo de información
					
					// Si no se puede leer el ID o tamaño, salir
					if (fread(info_id, 1, 4, f) != 4) 
						break;

					// Si no se puede leer el tamaño, salir
					if (fread(&info_size, 4, 1, f) != 1) {
						break;
					}

					info_id[4] = '\0';				// Asegurar terminación nula
					
					// Si el tamaño es inválido, salir
					if (info_size == 0 || info_size > 10000) {
						break;
					}
					
					// Calcular la posición del siguiente campo de información
					long next_info = ftell(f) + info_size;

					// Ajustar para padding de 2 bytes si es necesario
					if (info_size % 2 != 0) {
						next_info++;
					}
					
					// Leer el dato del campo de información
					char *info_data = (char*)malloc(info_size + 1);
					
					// Si no se pudo asignar memoria, salir
					if (!info_data) {
						break;
					}
					
					// Leer el dato del campo de información
					size_t bytes_read = fread(info_data, 1, info_size, f);
					int i;
					info_data[bytes_read] = '\0';
					
					// Eliminar caracteres nulos y espacios al final
					for (i = bytes_read - 1; i >= 0 && (info_data[i] == '\0' || info_data[i] == ' '); i--) {
						info_data[i] = '\0';
					}
					
					// Si el ID coincide con un campo conocido, guardarlo
					if (strcmp(info_id, "INAM") == 0 || strcmp(info_id, "TITL") == 0 || strcmp(info_id, "TIT2") == 0 || strcmp(info_id, "title") == 0) {
						strncpy(meta->titulo, info_data, sizeof(meta->titulo)-1);
						meta->titulo[sizeof(meta->titulo)-1] = '\0';
					
					// Si es artista, guardar
					} else if (strcmp(info_id, "IART") == 0 || strcmp(info_id, "TPE1") == 0 || strcmp(info_id, "artist") == 0) {
						strncpy(meta->artista, info_data, sizeof(meta->artista)-1);
						meta->artista[sizeof(meta->artista)-1] = '\0';

					// Si es album, guardar
					} else if (strcmp(info_id, "IPRD") == 0 || strcmp(info_id, "album") == 0 || strcmp(info_id, "product") == 0) {
						strncpy(meta->album, info_data, sizeof(meta->album)-1);
						meta->album[sizeof(meta->album)-1] = '\0';
					
						// Si es año, guardar
					} else if (strcmp(info_id, "ICRD") == 0 || strcmp(info_id, "date") == 0 || strcmp(info_id, "year") == 0) {
						strncpy(meta->ano, info_data, sizeof(meta->ano)-1);
						meta->ano[sizeof(meta->ano)-1] = '\0';
					}
					free(info_data);				// Liberar memoria
					fseek(f, next_info, SEEK_SET);	// Mover al siguiente campo de información
				}
			
			// Si no es un chunk INFO, saltar al siguiente chunk
			} else {
				fseek(f, next_chunk, SEEK_SET);
			}

		// Si no es un chunk conocido, saltar al siguiente chunk
		} else {
			fseek(f, next_chunk, SEEK_SET);
		}
	}

	// Calcular duración si tenemos todos los datos necesarios
	if (sample_rate > 0 && num_channels > 0 && bits_per_sample > 0 && data_size > 0) {

		// Calcular duración en segundos
		uint32_t bytes_per_sample = bits_per_sample / 8;
		
		// Calcular duración
		uint32_t total_samples = data_size / (num_channels * bytes_per_sample);
		
		// Formatear duración como MM:SS
		uint32_t duration_seconds = total_samples / sample_rate;
		uint32_t minutes = duration_seconds / 60;
		uint32_t seconds = duration_seconds % 60;

		// Guardar duración en metadatos
		snprintf(meta->duracion, sizeof(meta->duracion), "%u:%02u", minutes, seconds);
	}

	fclose(f);
}


int main() {
	const char *music_dir = "../../media/sd/music";
	DIR *dir;
	struct dirent *entry;
	FILE *json = fopen("canciones.json", "w");
	if (!json) {
		perror("No se pudo crear canciones.json");
		return 1;
	}
	fprintf(json, "{\n");
	int first = 1;

	dir = opendir(music_dir);
	if (dir == NULL) {
		perror("No se pudo abrir el directorio de música");
		fclose(json);
		return 1;
	}

	while ((entry = readdir(dir)) != NULL) {
		// Ignora "." y ".."
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;

		// Solo procesa archivos .wav
		size_t len = strlen(entry->d_name);
		if (len > 4 && strcmp(entry->d_name + len - 4, ".wav") == 0) {
			char filepath[512];
			snprintf(filepath, sizeof(filepath), "%s/%s", music_dir, entry->d_name);
			Metadatos meta;
			extraer_metadatos_wav(filepath, &meta);
			if (!first) fprintf(json, ",\n");
			first = 0;
			fprintf(json, "  ");
			escribir_json_string(json, entry->d_name);
			fprintf(json, ":\n");
			escribir_metadatos_json(json, &meta);
		}
	}
	fprintf(json, "\n}\n");
	closedir(dir);
	fclose(json);
	printf("Archivo canciones.json generado.\n");
	return 0;
}
