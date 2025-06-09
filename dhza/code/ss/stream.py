from google import genai
from google.genai import types
import os

# IMPORTANT: Replace "YOUR_API_KEY_HERE" with your actual Gemini API key.
# You can get one from Google AI Studio: [https://aistudio.google.com/app/apikey](https://aistudio.google.com/app/apikey)
# It's recommended to load your API key from an environment variable for security.
# For example: API_KEY = os.getenv("GEMINI_API_KEY")
API_KEY = "AIzaSyAm2S2vnvvXRmVe4KUtsYue30fJVo41yLA" # <--- REPLACE THIS!

# Initialize the Gemini client
try:
    client = genai.Client(api_key=API_KEY)
    print("[*] Gemini client initialized.")
except Exception as e:
    print(f"Error initializing Gemini client: {e}")
    print("Please ensure your API_KEY is correct and has access to Gemini.")
    exit()

# Define your prompt
prompt = "Tell me a short story about a cat who discovers a hidden magical garden."

print(f"\n[*] Sending prompt to AI: '{prompt}'")
print("[*] Receiving streaming response:")

try:
    # Request a streaming response from the model
    # Using gemini-2.5-flash-preview-05-20 as requested in previous context
    response_stream = client.models.generate_content_stream(
        model="gemini-2.5-flash-preview-05-20",
        contents=[prompt],
        # Optional: Configure the response generation
        config=types.GenerateContentConfig(
            # You can add other configurations here if needed, e.g., temperature
        )
    )

    # Iterate through the streamed chunks and print them
    for chunk in response_stream:
        # Each chunk has a 'text' attribute containing the part of the response
        print(chunk.text, end='', flush=True) # Use end='' and flush=True for real-time printing
    print("\n\n[*] Streaming complete.")

except Exception as e:
    print(f"\nError during streaming: {e}")
    print("Possible issues: Incorrect API key, network problems, or model access issues.")
