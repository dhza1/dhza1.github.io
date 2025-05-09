import os

for f in os.listdir("static"):
    if f.endswith(".mp3"):
        print(f'<audio autoplay src="static/{f}"></audio><br>')
