# Makefile para Audio Player Web Server
# Proyecto 2 - Sistemas Empotrados
#
# Targets:
#   make local   - Compilar para PC local (testing)
#   make arm     - Cross-compilar para ARM (DE1-SOC)
#   make test    - Compilar y ejecutar localmente
#   make clean   - Limpiar archivos generados
#   make deploy  - Preparar archivos para deployment

# Configuración
CC_LOCAL = gcc
CC_ARM = ../gcc-linaro-4.9-2016.02-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc

SRC_DIR = src
BUILD_DIR = build
WEB_DIR = web

SOURCES = $(SRC_DIR)/webserver.c $(SRC_DIR)/mongoose.c
INCLUDES = -I$(SRC_DIR)

# Flags de compilación
CFLAGS_COMMON = -Wall -O2 -DMG_ENABLE_PACKED_FS=0 -std=gnu99 -D_GNU_SOURCE
CFLAGS_LOCAL = $(CFLAGS_COMMON)
CFLAGS_ARM = $(CFLAGS_COMMON) -static

LDFLAGS = -lpthread

# Nombres de los binarios
BIN_LOCAL = $(BUILD_DIR)/webserver
BIN_ARM = $(BUILD_DIR)/webserver_arm

# Target por defecto
.PHONY: all
all: local

# Compilación local (para testing en PC)
.PHONY: local
local: $(BIN_LOCAL)

$(BIN_LOCAL): $(SOURCES)
	@echo "================================================"
	@echo "  Compilando para PC local (x86_64)..."
	@echo "================================================"
	@mkdir -p $(BUILD_DIR)
	$(CC_LOCAL) $(CFLAGS_LOCAL) $(INCLUDES) -o $@ $(SOURCES) $(LDFLAGS)
	@echo "✓ Compilación exitosa: $@"
	@echo ""

# Cross-compilación para ARM
.PHONY: arm
arm: $(BIN_ARM)

$(BIN_ARM): $(SOURCES)
	@echo "================================================"
	@echo "  Cross-compilando para ARM (DE1-SOC)..."
	@echo "================================================"
	@mkdir -p $(BUILD_DIR)
	$(CC_ARM) $(CFLAGS_ARM) $(INCLUDES) -o $@ $(SOURCES) $(LDFLAGS)
	@echo "✓ Cross-compilación exitosa: $@"
	@file $@
	@size $@
	@echo ""

# Compilar y ejecutar localmente
.PHONY: test
test: local
	@echo "================================================"
	@echo "  Iniciando servidor local..."
	@echo "================================================"
	@echo "Abre tu navegador en: http://localhost:8080"
	@echo "Presiona Ctrl+C para detener"
	@echo ""
	cd $(BUILD_DIR) && ./webserver

# Verificar que el binario ARM es estático
.PHONY: verify-arm
verify-arm: arm
	@echo "================================================"
	@echo "  Verificando binario ARM..."
	@echo "================================================"
	@echo "Tipo de archivo:"
	@file $(BIN_ARM)
	@echo ""
	@echo "Dependencias dinámicas:"
	@if command -v arm-linux-gnueabihf-readelf > /dev/null 2>&1; then \
		arm-linux-gnueabihf-readelf -d $(BIN_ARM) | grep NEEDED || echo "  ✓ Sin dependencias (estático)"; \
	else \
		echo "  (arm-linux-gnueabihf-readelf no disponible, omitiendo verificación)"; \
	fi
	@echo ""
	@echo "Tamaño del binario:"
	@ls -lh $(BIN_ARM) | awk '{print "  " $$9 ": " $$5}'
	@echo ""

# Preparar deployment
.PHONY: deploy
deploy: arm
	@echo "================================================"
	@echo "  Preparando archivos para deployment..."
	@echo "================================================"
	@mkdir -p $(BUILD_DIR)/deploy
	@mkdir -p $(BUILD_DIR)/deploy/www
	@cp $(BIN_ARM) $(BUILD_DIR)/deploy/webserver
	@cp $(WEB_DIR)/* $(BUILD_DIR)/deploy/www/
	@cp scripts/* $(BUILD_DIR)/deploy/ 2>/dev/null || true
	@echo "✓ Archivos preparados en: $(BUILD_DIR)/deploy/"
	@echo ""
	@echo "Contenido:"
	@ls -lh $(BUILD_DIR)/deploy/
	@echo ""
	@echo "Para copiar a la SD card:"
	@echo "  sudo cp -r $(BUILD_DIR)/deploy/* /mnt/sdcard/"
	@echo ""

# Limpiar archivos generados
.PHONY: clean
clean:
	@echo "Limpiando archivos generados..."
	@rm -rf $(BUILD_DIR)
	@echo "✓ Limpieza completa"

# Mostrar información de ayuda
.PHONY: help
help:
	@echo "================================================"
	@echo "  Audio Player Web Server - Makefile"
	@echo "================================================"
	@echo ""
	@echo "Targets disponibles:"
	@echo "  make local       - Compilar para PC local (testing)"
	@echo "  make arm         - Cross-compilar para ARM (DE1-SOC)"
	@echo "  make test        - Compilar y ejecutar localmente"
	@echo "  make verify-arm  - Verificar binario ARM (estático)"
	@echo "  make deploy      - Preparar archivos para deployment"
	@echo "  make clean       - Limpiar archivos generados"
	@echo "  make help        - Mostrar esta ayuda"
	@echo ""
	@echo "Ejemplos de uso:"
	@echo "  # Desarrollo local"
	@echo "  $$ make test"
	@echo ""
	@echo "  # Deployment a DE1-SOC"
	@echo "  $$ make arm"
	@echo "  $$ make verify-arm"
	@echo "  $$ make deploy"
	@echo ""
