Billboard japan Hot 100
から、2009~2025年の各週の
アーティスト名、トラック名、順位
を保存したcsvのフォルダ名が


billbord_scraping.py
↓
↓
★★★billboard_charts()★★★
↓
↓
song_filtering.py
↓
↓
filtered_billboard_charts
↓
↓
get_spotify_info.py
↓
↓
★★★extra_match_csv_of_spotify_info★★★
↓
↓
mp3_download.py
↓
↓
★★★csv_with_mp3_path★★★downloaded_mp3★★★
↓
↓
calculate_billboard_futures_info_by_age.py
↓
↓
★★★
billboard_futures_info_by_age
train
val
test
★★★


★★★csv_with_mp3_path★★★downloaded_mp3★★★
↓
↓
extract_complexity_features_from_mp3.py
extract_mfcc_features_from_mp3.py
↓
↓
★★★
features_complexity
features_mfcc
★★★


★★★
billboard_futures_info_by_age
train
val
test

features_complexity
features_mfcc
★★★
↓
↓
SVM
↓
↓
proceed？とかでグラフ化
