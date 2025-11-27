/*
 * JavaScript para Audio Player Web Interface
 * Actualiza la información en tiempo real desde el servidor
 */

// Elementos del DOM
const songElement = document.getElementById('song');
const artistElement = document.getElementById('artist');
const albumElement = document.getElementById('album');
const currentTimeElement = document.getElementById('current-time');
const totalTimeElement = document.getElementById('total-time');
const progressElement = document.getElementById('progress');
const statusElement = document.getElementById('status');
const trackNumberElement = document.getElementById('track-number');

// Función para obtener el estado actual del servidor
async function fetchStatus() {
    try {
        const response = await fetch('/api/status');
        if (!response.ok) {
            throw new Error('Error al obtener estado');
        }
        const data = await response.json();
        updateUI(data);
    } catch (error) {
        console.error('Error:', error);
        statusElement.textContent = '⚠️ Error de conexión';
    }
}

// Función para actualizar la interfaz con los datos recibidos
function updateUI(data) {
    // Actualizar información de la canción
    songElement.textContent = data.song;
    artistElement.textContent = data.artist;
    albumElement.textContent = data.album;
    
    // Actualizar tiempos
    currentTimeElement.textContent = data.current_time;
    totalTimeElement.textContent = data.total_time;
    
    // Actualizar barra de progreso
    progressElement.style.width = data.progress + '%';
    
    // Actualizar estado de reproducción
    if (data.is_playing) {
        statusElement.textContent = '▶️ Reproduciendo';
        statusElement.classList.add('playing');
    } else {
        statusElement.textContent = '⏸️ Pausado';
        statusElement.classList.remove('playing');
    }
    
    // Actualizar número de pista
    trackNumberElement.textContent = `Pista ${data.track}/${data.total_tracks}`;
}

// Actualizar cada segundo
setInterval(fetchStatus, 1000);

// Actualización inicial
fetchStatus();

// Mensaje de bienvenida en consola
console.log('🎵 Audio Player Web Interface iniciada');
console.log('Actualizando cada 1 segundo...');
