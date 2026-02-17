# 🎵 Billboard Japan Music Analysis Project (2009-2025)

Billboard Japan Hot 100のチャートデータから楽曲データ（曲名、アーティスト名、順位）を取得（フォルダ名：billboard_charts）。
SpotifyAPIと楽曲データ用いて、音源（mp3）を取得し、楽曲の複雑性特徴量とMFCC特徴量を抽出してSVMで解析するものである。

<br/>

## 最初に
+ 最初に自分の論文を読んで。わからない単語がある可能性があります。
+ SpotifyAPIを取得する必要があります（無料。2026/2/17時点）。
+ 入力先、出力先のフォルダとかは、プログラムの最初に定義しているので、必要がある場合変える。
+ データは全てSCVで保存される

<br/>


## ライブラリ

pandas, matplotlib, seaborn, os, glob, 
librosa, pandas, sklearn, spotipy, spotdl 等が必要です

<br/>


## 📊 データフロー図 (Data Pipeline)
<br/>

各スクリプトの実行順序と、生成されるディレクトリの関係を可視化しています。
<br/>


```mermaid
graph TD
    %% データの取得とフィルタリング
    Step1[billbord_scraping.py] -->|1. スクレイピング| Dir1[(billboard_charts<br/>)]
    
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
    Dir4 --> Step5[calculate_billboard_features_info.py]

    %%csvに保存する関数の種類（名前ごみでごめんなさ）
    Step5 -->|5.csvに保存する関数としてsave_split_csvを使う| Out1[(billboard_features/2008 ~ 2024 のフォルダ<br/>)]
    Step5 -->|5.csvに保存する関数としてsave_csvを使う| Dir7[(billboard_features_info/2008 ~ 2024.csv<br/>コイツにはmp3のpathが書いてある。)]

    %%音響的特徴量の抽出
    Dir7 --> Step6[extract_complexity_features.py]
    Dir7 --> Step7[extract_mfcc_features.py]

    %% 最終成果物
    %%Step5 -->|5. 分割データ| Out1[(billboard_features<br/>2008 ~ 2024)]
    Step6 -->|6. 複雑性特徴量| Out2[(features_complexity)]
    Step7 -->|6. MFCC特徴量| Out3[(features_mfcc)]

    %% 予測
    Out1 & Out2 & Out3 --> Step8[SVM.py]

    %% 結果
    Step8 --> |グリッドサーチを行った時のCとgammaの関係をしめした画像| Dir8[(plot__SVM)]
    Step8 --> Step9[(result_SVM)]

    %%可視化
    Step9 --> |フォルダ内にある複数のCSVファイルを読み込み、各指標ごとに平均を計算し、棒グラフで可視化するためには|　Dir10[plot_floder_metrics.py]
    Step9 --> |フォルダ内のcsvを読み込み,complexity+mfccのみを抽出して指標ごとに年代を決めて棒グラフで比較する| Dir11[analyze_combined_futuers.py]
    Step9 --> |#縦軸を正解度、横軸を年代として、指標ごとに比べる<br/>指標_実験_年代ごとに値が一番高かったものを表示する| Dir12[analyze_combined_futuers.py]

    %%style Dir1 fill:#f9f,stroke:#333
    style Dir4 fill:#f9f,stroke:#333
    style Out1 fill:#bbf,stroke:#333
    style Out2 fill:#bbf,stroke:#333
    style Out3 fill:#bbf,stroke:#333
```

<br/>

### データの説明
#### billboard_charts
+ 2008年～2025年の週間ごとのランキング情報
+ フォルダー/20xx/20xx_00_00.csv　の形
+ 順位,　曲名,　アーティスト名,　ランクスコア

#### filtering_billboard_charts
+ スクレイピングした情報から、3回以上登場した曲のみを抽出。
+ フォルダー/20xx/20xx_00_00.csv　の形
+ 順位,　曲名,　アーティスト名,　ランクスコア

#### extra_mathch_csv_of_spotify_info
+ SpotifyAPIを用いて、SpotifyのメタデータやURLを取得
+ フォルダー/20xx/20xx_00_00.csv　の形
+ アーティスト名,　曲名,　順位,　Spotify URL,　アーティスト人気度,　曲人気度
+ フォルダ名のextraだが、完全一致を表したもの。APIを使う時、曲名で検索して情報を取得している。ここで曲名が完全一致するものを取得するようにしている。gitを遡るとpartial_mathch_csv_of_spotify_infoがあるはず。
これは名前が部分一致したもの。（星野源には「ドラえもん」と「夢を叶えてドラえもん」が存在し、データを確実性をとった）

#### csv_with_mp3_path
+ spotdlを使用して、URLから音源を取得
+ 音源が保存してあるpathが書いてある
+ フォルダー/20xx/20xx_00_00.csv　の形
+ アーティスト名,曲名,順位,Spotify URL,アーティスト人気度,track人気度,MP3_Local_Path

#### billboard_features
+ calculate_billboard_features_info.pyで保存する関数save_split_csvを使って保存したもの
+ 年ごとにまとめた。
+ フォルダー/20xx.csv　の形

#### billboard_features_info
+ calculate_billboard_features_info.pyで保存する関数save_csvを使って保存したもの
+ 年ごとのファイルを一つにまとめたにまとめた。
+ フォルダー/20xx〜20xx.csv　の形

#### extract_complexity_features
+ complexity特徴量を抽出。どの曲がなんの特徴量を持っているかを調べるためにURLを持っている
+ Artist,Track,URL,MP3_Path　＋　特徴量

#### extract_mfcc_features
+ mfcc特徴量を抽出。どの曲がなんの特徴量を持っているかを調べるためにURLを持っている
+ Artist,Track,URL,MP3_Path　＋　特徴量

#### result_SVM
+ 正答率を保存
+ 横軸が使用した特徴量、縦軸が人気度指標
+ フォルダー/20xx~20xx.csv　の形
+ train + val + testで使った年の範囲になっている。 
+ 今のフォルダ内にはtrainに使った年や、グリッド法を使ったか、なのでフォルダーを変えているので、ぐちゃぐちゃに保存されている。



## 補足

+ send_email.py：これはモジュールとして使われている。実行時間が長いプログラムに入ってる。プログラムが終わったタイミングでgmailに送れる。自分のgmeilでidかなんかを作っていろいろしないといけないので使わなかったら使われているところをコメントアウトして。
+ for_features_csv.py：よく使う関数をモジュールとして保存した。どこに埋め込んでいるかわからなくて下手に消せない、、、
+ MPEG7：外部ソフトウェア。昔に作られていて、バージョンの互換性とか合わなくて使えない。