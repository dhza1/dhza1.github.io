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
playing = true;
function play_song() {
  if (playing) {
    // update the fucking song fucking label
    document.getElementById("songName").textContent = songs[index]
      .replace(".mp3", "")
      .replace("static/", "");
    // play the fucking song with the fucking index
    audio.src = songs[index];
    audio.play();
  } else {
    document.getElementById("songName").textContent = songs[index]
      .replace(".mp3", "")
      .replace("static/", "");
    audio.src = songs[index];
    audio.pause();
  }
}

function next_song() {
  if (index + 1 <= songs.length) {
    index++;
  } else {
    index = 0;
  }
  play_song();
}

function back_button() {
  if (index - 1 >= 0) {
    index--;
  } else {
    index = songs.length - 1;
  }
  play_song();
}

function play_button() {
  if (playing) {
    document.getElementById("pausebtn").textContent = "▶️";
    playing = false;
  } else {
    document.getElementById("pausebtn").textContent = "⏸️";
    playing = true;
  }
  play_song();
}
window.onload = function () {
  play_song();
  document.getElementById("Projects").value = "default";
};

function handleSelection(option) {
  console.log(option.value);
  if (option.value == "dobfuscator") {
    window.location.href = "https://github.com/dhza1/dobfuscator";
  } else if ((option.value = "archpkgmgr")) {
    window.location.href =
      "https://github.com/dhza1/dhza-stupid-package-manger";
  }
}
