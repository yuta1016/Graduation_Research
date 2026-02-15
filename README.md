# 🎵 Billboard Japan Music Analysis Project (2009-2025)

Billboard Japan Hot 100のチャートデータから楽曲データ（曲名、アーティスト名、順位）を取得。
SpotifyAPIと楽曲データ用いて、音源（mp3）を取得し、楽曲の複雑性特徴量とMFCC特徴量を抽出してSVMで解析するものである。

## ライブラリ

librosa, pandas, sklearn, spotipy 等が必要です

## 📊 データフロー図 (Data Pipeline)

各スクリプトの実行順序と、生成されるディレクトリの関係を可視化しています。


```mermaid
graph TD
    %% データの取得とフィルタリング
    Step1[billbord_scraping.py] -->|1. スクレイピング| Dir1[(billboard_charts)]
    Dir1 --> Step2[song_filtering.py]
    Step2 -->|2. 楽曲絞り込み| Dir2[(filtered_billboard_charts)]
    
    %% Spotifyメタ情報の取得
    Dir2 --> Step3[get_spotify_info.py]
    Step3 -->|3. メタデータ紐付け| Dir3[(extra_match_csv_of_spotify_info)]
    
    %% 音源ダウンロード
    Dir3 --> Step4[mp3_download.py]
    Step4 -->|4. 音源保存| Dir4[(downloaded_mp3)]
    Step4 -->|4. パス管理| Dir5[(csv_with_mp3_path)]

    %% 特徴量抽出とデータ分割
    Dir4 & Dir5 --> Step5[calculate_billboard.py]
    Dir4 & Dir5 --> Step6[extract_complexity_features.py]
    Dir4 & Dir5 --> Step7[extract_mfcc_features.py]

    %% 最終成果物
    Step5 -->|5. 分割データ| Out1[(billboard_futures<br/>train / val / test)]
    Step6 -->|6. 複雑性特徴量| Out2[(features_complexity)]
    Step7 -->|6. MFCC特徴量| Out3[(features_mfcc)]

    %% 解析
    Out1 & Out2 & Out3 --> Final[SVM Model & Visualization]

    style Dir1 fill:#f9f,stroke:#333
    style Dir4 fill:#f9f,stroke:#333
    style Out1 fill:#bbf,stroke:#333
    style Out2 fill:#bbf,stroke:#333
    style Out3 fill:#bbf,stroke:#333


