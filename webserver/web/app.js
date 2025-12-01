/*
 * JavaScript para Audio Player Web Interface
 * Actualiza la información en tiempo real desde el servidor
 */

// Elementos del DOM
const songElement = document.getElementById('song');
const artistElement = document.getElementById('artist');
const albumElement = document.getElementById('album');
const yearElement = document.getElementById('year');
const durationElement = document.getElementById('duration');
const filenameElement = document.getElementById('filename');
const currentTimeElement = document.getElementById('current-time');
const totalTimeElement = document.getElementById('total-time');
const progressElement = document.getElementById('progress');
const statusElement = document.getElementById('status');
const trackNumberElement = document.getElementById('track-number');
const nextBtn = document.getElementById('next-btn');
const prevBtn = document.getElementById('prev-btn');

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
    songElement.textContent = data.title;
    artistElement.textContent = data.artist;
    albumElement.textContent = data.album;
    yearElement.textContent = `Año: ${data.year}`;
    durationElement.textContent = `Duración: ${data.duration}`;
    filenameElement.textContent = `Archivo: ${data.filename}`;

    // Actualizar tiempos
    currentTimeElement.textContent = data.current_time;
    totalTimeElement.textContent = data.duration;

    // Actualizar barra de progreso
    progressElement.style.width = data.progress + '%';

    // Actualizar estado de reproducción según nios_action
    switch (data.nios_action) {
        case 1:
            statusElement.textContent = '▶️ Reproduciendo (NIOS)';
            statusElement.classList.add('playing');
            break;
        case 2:
            statusElement.textContent = '⏸️ Pausado (NIOS)';
            statusElement.classList.remove('playing');
            break;
        case 3:
            statusElement.textContent = '⏮️ Anterior (NIOS)';
            statusElement.classList.remove('playing');
            break;
        case 4:
            statusElement.textContent = '⏭️ Siguiente (NIOS)';
            statusElement.classList.remove('playing');
            break;
        default:
            // Mantener lógica original si no hay acción NIOS
            if (data.is_playing) {
                statusElement.textContent = '▶️ Reproduciendo';
                statusElement.classList.add('playing');
            } else {
                statusElement.textContent = '⏸️ Pausado';
                statusElement.classList.remove('playing');
            }
    }

    // Actualizar número de pista
    trackNumberElement.textContent = `Pista ${data.track}/${data.total_tracks}`;
}

// Funciones para cambiar de canción
async function nextSong() {
    try {
        await fetch('/api/next', { method: 'POST' });
        fetchStatus();
    } catch (error) {
        console.error('Error al cambiar a siguiente canción:', error);
    }
}

async function prevSong() {
    try {
        await fetch('/api/prev', { method: 'POST' });
        fetchStatus();
    } catch (error) {
        console.error('Error al cambiar a anterior canción:', error);
    }
}

if (nextBtn) nextBtn.addEventListener('click', nextSong);
if (prevBtn) prevBtn.addEventListener('click', prevSong);

// Actualizar cada segundo
setInterval(fetchStatus, 1000);

// Actualización inicial
fetchStatus();

// Mensaje de bienvenida en consola
console.log('🎵 Audio Player Web Interface iniciada');
console.log('Actualizando cada 1 segundo...');
