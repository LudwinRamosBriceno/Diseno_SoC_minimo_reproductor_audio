# Audio Player Web Server - DE1-SOC

Servidor web para mostrar información del reproductor de audio en tiempo real.  
Proyecto 2 - Sistemas Empotrados - Instituto Tecnológico de Costa Rica

## 📋 Descripción

Este proyecto implementa un servidor web ligero que corre en el procesador ARM Cortex-A9 del DE1-SOC, permitiendo visualizar la información del reproductor de audio (canción actual, artista, álbum, tiempo de reproducción) desde cualquier dispositivo con navegador web conectado a la misma red Ethernet.

### Características

- ✅ Servidor HTTP con Mongoose (single-file library)
- ✅ Interfaz web moderna y responsive
- ✅ Actualización en tiempo real (cada 1 segundo)
- ✅ Compilación estática (sin dependencias)
- ✅ Auto-inicio como daemon
- ✅ Footprint pequeño (~600KB-1.2MB)

## 🗂️ Estructura del Proyecto

```
webserver/
├── src/
│   ├── webserver.c         # Código principal del servidor
│   ├── mongoose.c          # Biblioteca HTTP
│   └── mongoose.h          # Headers de mongoose
├── web/
│   ├── style.css           # Estilos de la interfaz
│   └── app.js              # JavaScript para updates
├── build/
│   ├── webserver           # Binario x86_64 (testing local)
│   └── webserver_arm       # Binario ARM (deployment)
├── scripts/
│   ├── S99webserver        # Init script para daemon
│   └── deploy_to_sd.sh     # Script de deployment a SD
├── docs/
│   └── ...                 # Documentación adicional
├── Makefile                # Build system
└── README.md               # Este archivo
```

## 🔧 Requisitos

### Para desarrollo local (testing):
- gcc
- make
- pthread

### Para cross-compilación:
- Toolchain ARM: `gcc-linaro-4.9-2016.02-x86_64_arm-linux-gnueabihf`
- Ya incluido en el directorio del proyecto

### Para deployment:
- SD card con Linux mínimo (Buildroot/Yocto)
- Conexión Ethernet

## 🚀 Compilación

### 1. Testing Local (en tu PC)

```bash
# Compilar para x86_64
make local

# Compilar y ejecutar
make test

# El servidor estará disponible en:
# http://localhost:8080
```

### 2. Cross-compilación para ARM (DE1-SOC)

```bash
# Compilar para ARM
make arm

# Verificar que es estático
make verify-arm

# Salida esperada:
# webserver_arm: ELF 32-bit LSB executable, ARM, statically linked
```

### 3. Limpiar archivos generados

```bash
make clean
```

## 📦 Deployment a DE1-SOC

### Método 1: Script Automático (Recomendado)

```bash
# 1. Compilar para ARM
make arm

# 2. Montar la SD card
sudo mount /dev/sdX1 /mnt/sdcard  # Ajusta sdX según tu sistema

# 3. Ejecutar script de deployment
./scripts/deploy_to_sd.sh

# 4. El script te preguntará el punto de montaje
# Ingresa: /mnt/sdcard
```

### Método 2: Manual

```bash
# 1. Compilar
make arm

# 2. Preparar archivos
make deploy

# 3. Copiar a SD card (ajusta la ruta)
sudo cp build/webserver_arm /mnt/sdcard/usr/bin/webserver
sudo cp web/* /mnt/sdcard/var/www/
sudo cp scripts/S99webserver /mnt/sdcard/etc/init.d/
sudo chmod +x /mnt/sdcard/usr/bin/webserver
sudo chmod +x /mnt/sdcard/etc/init.d/S99webserver

# 4. Sincronizar y desmontar
sync
sudo umount /mnt/sdcard
```

## ⚙️ Configuración del Sistema (DE1-SOC)

### Red Ethernet

El servidor está configurado para usar:
- **IP**: 192.168.1.10
- **Puerto**: 8080
- **Interfaz**: eth0

Para cambiar la IP, edita `scripts/S99webserver`:

```bash
IP_ADDRESS=192.168.1.20  # Nueva IP
```

### Auto-inicio (Daemon)

El script `S99webserver` se ejecutará automáticamente al arrancar el sistema.

Para control manual en el DE1-SOC:

```bash
# Iniciar
/etc/init.d/S99webserver start

# Detener
/etc/init.d/S99webserver stop

# Reiniciar
/etc/init.d/S99webserver restart

# Ver estado
/etc/init.d/S99webserver status
```

### Ver logs

```bash
# En el DE1-SOC
tail -f /var/log/webserver.log
```

## 🌐 Uso

### 1. Conectar por Ethernet

**Opción A: Directo PC ↔ FPGA**

En tu PC, configura la interfaz Ethernet:

```bash
sudo ip addr add 192.168.1.100/24 dev eth0  # Ajusta 'eth0' a tu interfaz
```

**Opción B: Ambos en el mismo switch/router**

Ambos dispositivos recibirán IPs automáticamente (o configura estáticas en la misma subred).

### 2. Acceder a la interfaz web

Abre tu navegador:

```
http://192.168.1.10:8080
```

### 3. Visualización

La interfaz mostrará:
- 🎵 Nombre de la canción
- 🎤 Artista
- 💿 Álbum
- ⏱️ Tiempo actual / total
- 📊 Barra de progreso
- ▶️ Estado (reproduciendo/pausado)
- 🎼 Número de pista

La información se actualiza **cada segundo** automáticamente.

## 🔌 Integración con FPGA

### Comunicación HPS ↔ FPGA

El servidor está preparado para leer información desde la FPGA. Necesitas implementar la lectura de datos según tu diseño:

**Opción 1: Memoria compartida (Lightweight HPS-to-FPGA bridge)**

```c
// En webserver.c, función update_audio_info():

// Direcciones de memoria mapeada
#define FPGA_BASE_ADDR 0xFF200000
#define AUDIO_TIME_OFFSET 0x00
#define AUDIO_STATUS_OFFSET 0x04

volatile uint32_t *fpga_mem = (uint32_t *)mmap(NULL, 4096, 
    PROT_READ | PROT_WRITE, MAP_SHARED, fd, FPGA_BASE_ADDR);

// Leer tiempo actual desde FPGA
audio_info.current_time_sec = *(fpga_mem + AUDIO_TIME_OFFSET);
audio_info.is_playing = *(fpga_mem + AUDIO_STATUS_OFFSET);
```

**Opción 2: PIOs (Parallel I/O)**

```c
// Mapear PIOs de Platform Designer
#define PIO_AUDIO_TIME_BASE 0xFF200000
#define PIO_AUDIO_STATUS_BASE 0xFF200010

// Leer desde PIO
audio_info.current_time_sec = *((volatile uint32_t *)PIO_AUDIO_TIME_BASE);
```

**Opción 3: Mailbox o FIFO**

Implementa un mecanismo de comunicación según tu diseño en Platform Designer.

### Ejemplo de integración completa

Ver archivo: `docs/fpga_integration_example.c` (próximamente)

## 🧪 Testing

### Test local

```bash
make test
# Abre http://localhost:8080
# Verifica que la interfaz carga correctamente
# Observa que los datos se actualizan cada segundo
```

### Test en DE1-SOC

1. Conecta por Ethernet
2. Abre http://192.168.1.10:8080
3. Verifica conectividad:
   ```bash
   ping 192.168.1.10
   ```
4. Si no responde, verifica en el DE1-SOC:
   ```bash
   ifconfig eth0
   netstat -tuln | grep 8080
   /etc/init.d/S99webserver status
   ```

## 📊 Endpoints API

### GET /

Interfaz web principal (HTML)

### GET /api/status

Retorna el estado actual en JSON:

```json
{
  "song": "Bohemian Rhapsody",
  "artist": "Queen",
  "album": "A Night at the Opera",
  "current_time": "01:23",
  "total_time": "05:55",
  "current_sec": 83,
  "total_sec": 355,
  "is_playing": 1,
  "progress": 23,
  "track": 1,
  "total_tracks": 5
}
```

### GET /style.css

Hoja de estilos

### GET /app.js

JavaScript de la aplicación

## 🐛 Troubleshooting

### El servidor no inicia en el DE1-SOC

```bash
# Verificar que el binario existe y es ejecutable
ls -l /usr/bin/webserver
file /usr/bin/webserver

# Verificar permisos
chmod +x /usr/bin/webserver

# Ejecutar manualmente para ver errores
/usr/bin/webserver
```

### No puedo acceder desde el navegador

```bash
# En el DE1-SOC, verificar IP
ifconfig eth0

# Verificar que el servidor está escuchando
netstat -tuln | grep 8080

# En tu PC, verificar conectividad
ping 192.168.1.10

# Verificar firewall (si aplica)
sudo iptables -L
```

### El binario ARM es muy grande

```bash
# Verificar que compilaste con -static
make verify-arm

# Optimizar tamaño (opcional, menos compatible)
# En Makefile, cambiar:
CFLAGS_ARM = $(CFLAGS_COMMON) -static -Os -s
```

### Errores de cross-compilación

```bash
# Verificar que el toolchain existe
ls -l ../gcc-linaro-4.9-2016.02-x86_64_arm-linux-gnueabihf/bin/

# Verificar path en Makefile
# Línea: CC_ARM = ../gcc-linaro...
```

## 📝 Notas Importantes

1. **Memoria**: El servidor usa ~2-3MB de RAM en runtime
2. **CPU**: Carga mínima (~1-2% en idle)
3. **Network**: Solo funciona con Ethernet (no WiFi en DE1-SOC)
4. **Seguridad**: Este es un servidor básico sin autenticación (solo para uso local)
5. **Concurrencia**: Mongoose maneja múltiples conexiones simultáneas

## 🔮 Próximos Pasos

- [ ] Implementar integración con FPGA (memoria compartida)
- [ ] Agregar soporte para controlar reproducción desde web (play/pause/skip)
- [ ] Implementar WebSockets para updates más eficientes
- [ ] Agregar carga de canciones desde SD card
- [ ] Optimizar tamaño del binario

## 📚 Referencias

- [Mongoose Web Server](https://mongoose.ws/)
- [DE1-SOC User Manual](https://www.terasic.com.tw/cgi-bin/page/archive.pl?No=836)
- [ARM Cross-compilation Guide](https://developer.arm.com/)

## 👥 Autores

Proyecto 2 - Sistemas Empotrados  
Instituto Tecnológico de Costa Rica  
2025

## 📄 Licencia

Este proyecto es para uso académico.
