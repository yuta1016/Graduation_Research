import glob
import random
import numpy as np
import librosa
from sklearn.cluster import KMeans
import for_features_csv
import send_mail

INPUT_DATA_FOLDER = "billboard_futures_info"
#INPUT_DATA_FOLDER = "test"
OUTPUT_DATA_FOLDER = "features_mfcc"
FILE_NAME = "2008_2025_mfcc.csv"

class AudioFeatureExtractor:
    def __init__(self, n_clusters=32, sr=22050):
        """
        Args:
            n_clusters (int): クラスタ数 (論文に基づき 32 に設定)
            sr (int): サンプリングレート
        """
        self.n_clusters = n_clusters
        self.sr = sr
        self.kmeans = None

    def compute_mfcc(self, file_path):
        """
        MFCC抽出処理
        - Window length: 0.025 sec (25ms)
        - Overlap: 0.015 sec (15ms) -> Hop length: 0.010 sec (10ms)
        - MFCCs: 20 dimensions (0次元目を捨てるため実質19次元を使用と仮定)
        """
        try:
            y, sr = librosa.load(file_path, sr=self.sr)

            """pecifically, we first convert each song in our training set to a series of 20 dimensional MFCC vectors computed from overlapping 25ms windows sampled each 10ms. W"""
            # --- ウィンドウ設定の計算 ---
            # Window length: 0.025 sec
            n_fft = int(0.025 * sr)
            
            # Hop length = Window - Overlap
            # 0.025 - 0.015 = 0.010 sec (10ms)
            hop_length = int(0.010 * sr)

            # --- MFCC計算 ---
            mfcc = librosa.feature.mfcc(
                y=y, sr=sr,
                n_mfcc=20,       # 20次元抽出
                n_fft=n_fft,     # ウィンドウサイズ
                hop_length=hop_length # 移動幅
            )

            #print(mfcc)
            #print(mfcc.shape)
            
            """ discard the 0th (DC) component of each vector then perform K-means clustering to learn the N most prominent sounds. """
            # --- 0次元目(DC成分/パワー)の削除 ---
            # 0行目だけ大きな負の値だった
            # shape: (20, Time) -> (19, Time)
            mfcc_filtered = mfcc[1:, :]

            # 転置して (Time, 19) にする (scikit-learnの入力形式に合わせる)
            return mfcc_filtered.T

        except Exception as e:
            print(f"Error processing {file_path}: {e}")
            return None


    def learn_clusters(self, training_files, sample_size=200):#200
        """
        Step 1: K-meansで「代表的な音のパターン(Centroids)」を32個学習する
        """
        print(f"学習用データの準備中 (サンプル数: {sample_size}曲)...")
        
        # 学習用ファイルをランダムサンプリング
        if len(training_files) > sample_size:
            sampled_files = random.sample(training_files, sample_size)
        else:
            sampled_files = training_files

        all_features = []
        for file_path in sampled_files:
            mfcc = self.compute_mfcc(file_path)
            if mfcc is not None:
                all_features.append(mfcc)

        if not all_features:
            raise ValueError("特徴量が抽出できませんでした。音声ファイルを確認してください。")

        # 全曲のフレームを縦に結合して巨大な行列にする
        stacked_features = np.vstack(all_features)
        
        """We discard the 0th (DC) component of each vector then perform K-means clustering to learn the N most prominent sounds."""
        print(f"K-meansクラスタリング実行中 (クラスタ数: {self.n_clusters})...")
        # K-means実行
        self.kmeans = KMeans(n_clusters=self.n_clusters, random_state=42, n_init=10)
        self.kmeans.fit(stacked_features)
        print("学習完了。32個の代表的な音響パターンを獲得しました。\n")


    def extract_feature_vector(self, url, file_path):
        """
        Step 2: 対象の曲を32次元の特徴量ベクトルに変換する
        (normalized frequencies of the 32 clusters)
        """
        if self.kmeans is None:
            raise Exception("モデル未学習です。先に learn_clusters を実行してください。")

        # MFCC抽出
        features = self.compute_mfcc(file_path)
        if features is None:
            return None

        # --- 最近傍クラスタの探索 ---
        # 各フレームが32個のパターンのうちどれに一番近いかを判定
        cluster_labels = self.kmeans.predict(features)

        """For each vector, we score it against each of the N clusters and increment a counter for the cluster which scores highest. """
        # --- ヒストグラム作成 ---
        # 0番〜31番のクラスタがそれぞれ何回登場したかをカウント
        counts = np.bincount(cluster_labels, minlength=self.n_clusters)

        """ The normalized set of counts forms the N-dimensional representation for that song."""
        # --- 正規化 (Normalized frequencies) ---
        # 合計が1になるように割る (L1正規化)
        normalized_vector = {}
        if np.sum(counts) > 0:
            normalized_vector[url] = counts.astype(float) / np.sum(counts)
        else:
            normalized_vector[url] = np.zeros(self.n_clusters)

        return normalized_vector


if __name__ == "__main__":
    FeaturesExcelSaver = for_features_csv.FeaturesExcelSaver()

    csv_files = FeaturesExcelSaver.find_input_csv_files(INPUT_DATA_FOLDER)
    mp3_paths_dict, csv_data = FeaturesExcelSaver.retrive_mp3_path(csv_files, 3, 2)  # 引数1個目がmp3_parth, 2個目がURL
    training_files = list(mp3_paths_dict.values())

    #print(training_files)
    #print(len(training_files))

    print("--- プログラム設定 ---")
    print(f"Cluster N: 32")
    print(f"Window: 25ms / Overlap: 15ms")
    
    extractor = AudioFeatureExtractor(n_clusters=32)
    
    # 1. 学習 (コードブック作成)
    extractor.learn_clusters(training_files)
    
    # 2. 特徴抽出 (各曲を32次元ベクトル化)
    # target_song = "./downloaded_mp3/星野源_ドラえもん.mp3"
    # feature_vector = extractor.extract_feature_vector(target_song)
    features_dict = {}
    for url, target_song in mp3_paths_dict.items():
        print(f"\nProcessing mp3_path: {target_song}")
        feature_dict = extractor.extract_feature_vector(url, target_song)
        features_dict.update(feature_dict)#{URL: np.array([...])}

        # 辞書から配列(numpy array)を取り出す
        vector_array = feature_dict[url]

        print(f"抽出された特徴量: \n{feature_dict}") # 全体を表示
        print(f"次元数: {len(vector_array)}") # これで 32 になります
    
    FeaturesExcelSaver.save_features_to_excel(features_dict, csv_data, OUTPUT_DATA_FOLDER, FILE_NAME)
    send_mail.prosess_mail("MFCC特徴量の抽出が完了しました。")
