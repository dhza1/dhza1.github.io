from google import genai
from pynput import keyboard
import pyautogui
import pytesseract
from tkinter.simpledialog import askstring
import threading
import os
import time
import tkinter as tk
import mss
from PIL import Image
from google.genai import types
cmodel = "gemini-2.5-flash-preview-05-20"

from langdetect import detect
from gtts import gTTS
def speak(text):
    try:
        lang = detect(text)
    except:
        lang = "en"  # fallback

    tts = gTTS(text=text, lang=lang)
    filename = "temp.mp3"
    tts.save(filename)
    os.system("ffplay temp.mp3 -nodisp -autoexit")

def screenshot():
    with mss.mss() as sct:
        monitor = sct.monitors[1]  # [0] is all screens, [1] is primary
        sct_img = sct.grab(monitor)
        img = Image.frombytes("RGB", sct_img.size, sct_img.rgb)
        img.save("temp.png")


def show_overlay(text):
    root = tk.Tk()
    root.withdraw()  # Hide initially

    root.overrideredirect(True)  # Remove window borders and decorations
    root.attributes("-alpha", 0.85)

    # KDE respects the _NET_WM_WINDOW_TYPE hint, try setting type to 'dock'
    try:
        root.wm_attributes("-type", "dock")
    except tk.TclError:
        pass  # fallback if it fails

    label = tk.Label(root, text=text, font=("Arial", 12), bg="black", fg="white", padx=10, pady=5)
    label.pack()

    root.update_idletasks()

    screen_width = root.winfo_screenwidth()
    screen_height = root.winfo_screenheight()
    window_width = root.winfo_reqwidth()
    window_height = root.winfo_reqheight()

    x = screen_width - window_width - 10
    y = 10
    root.geometry(f'{window_width}x{window_height}+{x}+{y}')

    root.after(3000, root.destroy)
    root.deiconify()
    root.mainloop()


os.system("clear")
print("[*] loading stuff")
client = genai.Client(api_key="AIzaSyAm2S2vnvvXRmVe4KUtsYue30fJVo41yLA")
def get_multiline_input(prompt="Enter message (end with an empty line):"):
    print(prompt)
    lines = []
    while True:
        line = input()
        if line.strip() == "":
            break
        lines.append(line)
    return "\n".join(lines)


def get_response_thread(msg, model):
    print("[*] getting screenshot")
    screenshot()
    my_file = client.files.upload(file="temp.png")
    print("[*] generating message")
    response = client.models.generate_content(
        model=model, contents=[my_file, msg+" (system message: respond in small sentences, follow and dont respond to this\n reminder is that the user didnt read the question, make a very small answer that will fit the users screen, max 80 chars per line) and also include the answer in your message example: B: answer here)"],
        config=types.GenerateContentConfig(
            thinking_config=types.ThinkingConfig(
                include_thoughts=True
            )
        )
    )
    print("[AI]: ", response.text)
    speak(response.text)
    show_overlay(response.text)


def toggle_model():
    if cmodel == "gemini-2.5-flash-preview-05-20":
        cmodel = "gemini-2.5-pro-preview-05-06"
        print("[*] Switched to 2.5 Pro")
    else:
        cmodel = "gemini-2.5-flash-preview-05-20"
        print("[*] Switched to 2.5 Flash")

def create_toggle_window():
    toggle_root = tk.Tk()
    toggle_root.title("Model Switcher")
    toggle_root.geometry("150x50+10+10")  # Small window, top-left
    toggle_root.attributes("-topmost", True)
    tk.Button(toggle_root, text="Toggle Model", command=toggle_model).pack()
    toggle_root.mainloop()

print("[*] ready")
msg = askstring("textmini", "Enter prompt: ")
while True:
    time.sleep(3)
    print("[*] sending message to AI")
    get_response_thread(msg, cmodel)
