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
    Step4 -->|4. パス管理| Dir4[(csv_with_mp3_path)]
    Step4 -->|4. 音源保存| Dir5[(downloaded_mp3)]
    

    %% 特徴量抽出とデータ分割
    Dir4 --> Step5[calculate_billboard_futures_info.py]

    %%csvに保存する関数の種類（名前ごみでごめんなさ）
    Step5 -->|5.csvに保存する関数としてsave_split_csvを使う| Out1[(billboard_futures<br/>2008 ~ 2024 のフォルダ<br/>)]
    Step5 -->|5.csvに保存する関数としてsave_csvを使う| Dir7[(billboard_futures_info<br/>2008 ~ 2024.csv<br/>コイツにはmp3のpathが書いてある。)]

    %%音響的特徴量の抽出
    Dir7 --> Step6[extract_complexity_features.py]
    Dir7 --> Step7[extract_mfcc_features.py]

    %% 最終成果物
    %%Step5 -->|5. 分割データ| Out1[(billboard_futures<br/>2008 ~ 2024)]
    Step6 -->|6. 複雑性特徴量| Out2[(features_complexity)]
    Step7 -->|6. MFCC特徴量| Out3[(features_mfcc)]

    %% 予測
    Out1 & Out2 & Out3 --> Step8[SVM.py]

    %% 結果
    Step8 --> Step9[(result_SVM)]

    %%可視化
    Step9 --> |フォルダ内にある複数のCSVファイルを読み込み、各指標ごとに平均を計算し、棒グラフで可視化するためには|　Dir10[plot_floder_metrics.py]
    Step9 --> |フォルダ内のcsvを読み込み,complexity+mfccのみを抽出して指標ごとに年代を決めて棒グラフで比較する| Dir11[analyze_combined_futuers.py]
    Step9 --> |#縦軸を正解度、横軸を年代として、指標ごとに比べる<br/>指標_実験_年代ごとに値が一番高かったものを表示する| Dir12[analyze_combined_futuers.py]

    style Dir1 fill:#f9f,stroke:#333
    style Dir4 fill:#f9f,stroke:#333
    style Out1 fill:#bbf,stroke:#333
    style Out2 fill:#bbf,stroke:#333
    style Out3 fill:#bbf,stroke:#333


