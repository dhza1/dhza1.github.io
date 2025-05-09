function redirect_discord() {
  window.location.href = "https://discord.gg/WEhR5uZS";
}

const songs = [
  "static/умираю (prod. Dj fullcola666 ).mp3",
  "static/Walk in the cold - Dayerteq.mp3",
  "static/PDD - dj trippie flameboy.mp3",
  "static/Ева - Vintage.mp3",
  "static/[DJ CIGAN X DJ PEDOPHILE X DJ PIČKA] KAI ANGEL - MILLIONS.mp3",
  "static/Take The L - dj trippie flameboy.mp3",
];

function redirect_github() {
  window.location.href = "https://github.com/dhza1";
}

function get_random_song() {
  return songs[Math.floor(Math.random() * songs.length)];
}

const audio = new Audio();

function play_random_song() {
  const song = get_random_song();
  console.log(song);
  audio.src = song;
  audio.play();
}
index = 0;
max_index = songs.length;
function play_song() {
  // update the fucking song fucking label
  document.getElementById("songName").textContent = songs[index]
    .replace(".mp3", "")
    .replace("static/", "");
  // play the fucking song with the fucking index
  audio.src = songs[index];
  audio.play();
}

function next_song() {
  if (index + 1 <= songs.length) {
    index++;
    play_song();
  } else {
    index = 0;
    play_song();
  }
}
