function redirect_discord() {
  window.location.href = "https://discord.gg/WEhR5uZS";
}

const songs = [
  "static/умираю (prod. Dj fullcola666 ).mp3",
  "static/Take The L.mp3",
  "static/Walk in the cold.mp3",
  "static/Ева - Vintage (_lonely lonely I guess I_m lonely_) [HARDSTYLE] (SLOWED_REVERB).mp3",
  "static/[DJ CIGAN X DJ PEDOPHILE X DJ PIČKA] KAI ANGEL - MILLIONS.mp3",
  "static/PDD.mp3",
];

function redirect_github() {
  window.location.href = "https://github.com/dhza1";
}

function get_random_song() {
  return songs[Math.random() * songs.length];
}

const audio = new Audio();

function play_random_song() {
  const song = get_random_song();
  audio.src = song;
  audio.play();
}

audio.addEventListener("ended", play_random_song());

window.onload = play_random_song();
