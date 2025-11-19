"""
https://global.php.cn/ja/faq/1796615471.html

https://qiita.com/daisei0311/items/3b32a16faf3c2de826b9
"""

import subprocess

 

def download_spotify_mp3():

    print("Spotify to MP3 Downloader")

 

    content_type = input("What do you want to download? (Enter 'track', 'playlist', or 'album'): ").strip().lower()

 

    if content_type not in ['track', 'playlist', 'album']:

        print("Invalid choice. Please enter 'track', 'playlist', or 'album'.")

        return

 

    spotify_url = input(f"Enter the Spotify {content_type} URL: ").strip()

 

    try:

        print(f"\nDownloading {content_type} as MP3...")

        subprocess.run(["spotdl", "--format", "mp3", spotify_url])

        print(f"\nDownload of {content_type} completed in MP3 format!\n")

 

    except Exception as e:

        print(f"An error occurred: {e}")

 

if __name__ == "__main__":

    download_spotify_mp3()