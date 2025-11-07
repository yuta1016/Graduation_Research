import spotipy
from spotipy.oauth2 import SpotifyClientCredentials
import csv
import time
import os
from dotenv import load_dotenv
load_dotenv()


# --- 設定 ---
# Spotify APIのクライアントIDとシークレットを環境変数から取得
CLIENT_ID = os.getenv("CLIENT_ID")
CLIENT_SECRET = os.getenv("CLIENT_SECRET")

INPUT_CSV_FILE = './test_csv/input_data.csv'
OUTPUT_CSV_FILE = 'output_spotify_urls.csv'
# -----------


def setup_spotify_client(client_id, client_secret):
    """Spotifyクライアントを設定し、認証マネージャーを返す"""
    if client_id == "YOUR_CLIENT_ID" or client_secret == "YOUR_CLIENT_SECRET":
        print("エラー: CLIENT_ID または CLIENT_SECRET が設定されていません。")
        return None
    
    client_credentials = SpotifyClientCredentials(
        client_id=client_id,
        client_secret=client_secret
    )
    return spotipy.Spotify(client_credentials_manager=client_credentials)

def search_artist_id(sp, artist_name):
    """
    アーティスト名で検索し、SpotifyのアーティストIDを返す
    (表記ゆれに対応するため、検索結果の最初のIDを使う)
    """
    query = f"{artist_name}"
    try:
        results = sp.search(q=query, type='artist', limit=4)
        if results['artists']['items']:
            artist_id = results['artists']['items'][0]['id']
            spotify_name = results['artists']['items'][0]['name']
            artist_popularity = results['artists']['items'][0]['popularity']

            print(f"デバッグ: アーティストID取得: 入力名='{artist_name}' -> Spotify名='{spotify_name}', ID='{artist_id}'")
            #print(results)
            return artist_id
        return None
    except Exception as e:
        print(f"エラー: アーティストID検索中に問題が発生しました ({artist_name}): {e}")
        return None
    finally:
        time.sleep(0.5) # APIリクエスト間隔を空ける

def search_and_get_url(sp, artist_name, track_name, target_artist_id):
    """
    曲名、アーティスト名で検索し、事前に取得したアーティストIDと一致するトラックのURLを返す。
    """
    # トラック名のみで検索クエリを作成
    query = f"track:{track_name} artist:{artist_name}"
    
    try:
        print(f"デバッグ: トラック検索クエリ: '{query}' を実行中...")
        results = sp.search(q=query, type='track', limit=10)
        
        found_url = None
        
        # 検索結果をループして、アーティストIDが一致するものを探す
        for track in results['tracks']['items']:
            # 検索結果のトラック名を取得
            result_track_name = track['name'].strip()
            
            # 検索結果のアーティストIDを取得
            # トラックが複数のアーティストを持つ場合もあるが、ここでは最初のIDを取得
            result_artist_id = track['artists'][0]['id'] if track['artists'] else None
            
            # --- 照合ロジック ---
            # 1. 曲名が完全一致すること (小文字比較で表記ゆれを吸収)
            # 2. アーティストIDが事前に検索したIDと一致すること
            if (result_track_name.lower() == track_name.lower()) and \
               (result_artist_id == target_artist_id):
                
                # 完全に一致した場合、URLを保存してループを抜ける
                found_url = track['external_urls']['spotify']
                break
            else:
                # デバッグ: 一致しなかった場合の情報を表示
                debug_artist_name = track['artists'][0]['name'] if track['artists'] else "N/A"
                print(f"デバッグ: 照合失敗: 曲名='{result_track_name}', アーティスト名='{debug_artist_name}' (ID: {result_artist_id})")

        return found_url
        
    except Exception as e:
        print(f"エラーが発生しました (曲名: {track_name}): {e}")
        return None
    finally:
        time.sleep(0.5) # APIリクエスト間隔を空ける

def main():
    sp = setup_spotify_client(CLIENT_ID, CLIENT_SECRET)
    if sp is None:
        return

    print("--- Spotify URL取得プログラムを開始（照合強化版） ---")

    if not os.path.exists(INPUT_CSV_FILE):
        print(f"エラー: 入力ファイル '{INPUT_CSV_FILE}' が見つかりません。")
        return

    # 入力CSVを読み込み
    try:
        with open(INPUT_CSV_FILE, mode='r', encoding='utf-8') as infile:
            reader = csv.reader(infile)
            header = next(reader)
            songs_to_search = list(reader)
    except Exception as e:
        print(f"エラー: CSVファイルの読み込み中に問題が発生しました。{e}")
        return

    # 1. アーティスト名からIDを事前に取得するフェーズ
    artist_ids = {}
    unique_artists = sorted(list(set(row[1].strip() for row in songs_to_search if len(row) >= 2)))

    print(f"\n--- 1. アーティストIDの事前取得 ({len(unique_artists)}名) ---")
    for artist in unique_artists:
        artist_id = search_artist_id(sp, artist)
        if artist_id:
            artist_ids[artist] = artist_id
        else:
            print(f"警告: アーティスト '{artist}' のIDが見つかりませんでした。このアーティストの楽曲は検索できません。")
            artist_ids[artist] = None

    # 2. 楽曲ごとに検索を実行するフェーズ
    results_data = []
    total_songs = len(songs_to_search)
    print(f"\n--- 2. 楽曲のURL検索 ({total_songs}件) ---")
    
    for index, row in enumerate(songs_to_search):
        if len(row) < 2:
            results_data.append([row[0].strip() if row else "N/A", "N/A", "スキップされました（データ不足）"])
            continue
            
        artist = row[1].strip()
        track = row[2].strip()
        target_id = artist_ids.get(artist)
        spotify_url = None

        if target_id:
            # IDを使ってトラックを検索
            spotify_url = search_and_get_url(sp, artist, track, target_id)
        
        # 結果をリストに追加
        results_data.append([artist, track, spotify_url if spotify_url else "URLが見つかりません"])
        
        print(f"進行状況: {index + 1}/{total_songs} - {artist} - {track}: {'取得完了' if spotify_url else '見つからず'}")

    # 3. 結果をCSVファイルに書き込み
    output_header = ['アーティスト名', '曲名', 'Spotify URL']
    try:
        with open(OUTPUT_CSV_FILE, mode='w', newline='', encoding='utf-8') as outfile:
            writer = csv.writer(outfile)
            writer.writerow(output_header)
            writer.writerows(results_data)
        
        print(f"\n--- 完了 ---")
        print(f"結果がファイル '{OUTPUT_CSV_FILE}' に保存されました。")
    except Exception as e:
        print(f"エラー: 結果のCSVファイル書き出し中に問題が発生しました。{e}")

if __name__ == "__main__":
    main()