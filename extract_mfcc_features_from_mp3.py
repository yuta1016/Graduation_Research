import os
import random
import numpy as np
import librosa
from sklearn.cluster import KMeans

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

            # --- 0次元目(DC成分/パワー)の削除 ---
            # shape: (20, Time) -> (19, Time)
            mfcc_filtered = mfcc[1:, :]

            # 転置して (Time, 19) にする (scikit-learnの入力形式に合わせる)
            return mfcc_filtered.T

        except Exception as e:
            print(f"Error processing {file_path}: {e}")
            return None

    def learn_clusters(self, training_files, sample_size=200):
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
        
        print(f"K-meansクラスタリング実行中 (クラスタ数: {self.n_clusters})...")
        # K-means実行
        self.kmeans = KMeans(n_clusters=self.n_clusters, random_state=42, n_init=10)
        self.kmeans.fit(stacked_features)
        print("学習完了。32個の代表的な音響パターンを獲得しました。")

    def extract_feature_vector(self, file_path):
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

        # --- ヒストグラム作成 ---
        # 0番〜31番のクラスタがそれぞれ何回登場したかをカウント
        counts = np.bincount(cluster_labels, minlength=self.n_clusters)

        # --- 正規化 (Normalized frequencies) ---
        # 合計が1になるように割る (L1正規化)
        if np.sum(counts) > 0:
            normalized_vector = counts.astype(float) / np.sum(counts)
        else:
            normalized_vector = np.zeros(self.n_clusters)

        return normalized_vector

# --- 使い方 (ダミーデータでの例) ---
if __name__ == "__main__":
    # 本来はここに音声ファイルパスのリストが入ります
    # import glob
    # training_files = glob.glob("./dataset/*.mp3")
    
    # ダミーのパスリスト（動作イメージ用）
    training_files = ["song1.mp3", "song2.mp3"] 
    
    print("--- プログラム設定 ---")
    print(f"Cluster N: 32")
    print(f"Window: 25ms / Overlap: 15ms")
    
    # 実際のファイルがないと動かないため、使い方のフローだけ示します
    """
    extractor = AudioFeatureExtractor(n_clusters=32)
    
    # 1. 学習 (コードブック作成)
    extractor.learn_clusters(training_files)
    
    # 2. 特徴抽出 (各曲を32次元ベクトル化)
    target_song = "new_song.mp3"
    feature_vector = extractor.extract_feature_vector(target_song)
    
    print(f"抽出された特徴量: {feature_vector}")
    print(f"次元数: {len(feature_vector)}") # 32になるはず
    """




    """
    [21]では、ヒット曲予測のために楽曲のスペクトル特性を測定するMFCCベースの特徴量が用いられた。我々も[21]の特徴抽出手順を実装してこれを採用する。各音声信号は0.025秒の長さのセグメントに分割され、隣接セグメント間には0.015秒のオーバーラップが設けられ、各セグメントから20個のMFCCが抽出される。
    トレーニングデータセット内の楽曲から抽出したMFCCに対し、
    ノイズ低減と特徴のコンパクト化を目的としてk-meansクラスタリングを実施し、
    32個のクラスター中心点を取得する。これらの中心点はトレーニングデータに
    最も頻繁に見られる音響特性を表す。テスト用楽曲が与えられると、
    そのMFCCベクトルに対する最小距離クラスタ中心が特定され、
    32クラスタの正規化周波数が楽曲の特徴量として取得される。
    """

    """
    まず、1700曲の実験データベースの各楽曲を音響的表現と歌詞ベースの表現に変換します。
    前述の通り、各楽曲を音響的表現に変換する最初のステップは、
    一般的な音声データセット(traning copra)内で最も顕著なN個のクラスターを学習することです。具体的には、
    まずトレーニングセットの各楽曲を、10ms間隔でサンプリングした25msのオーバーラップするウィンドウから計算された
    20次元のMFCCベクトルの系列に変換します。各ベクトルの0番目（DC）成分を除去した後、
    K-meansクラスタリングを実行してN個の最も顕著な音声を学習する。計算上の理由から、
    音響データを持つ全18,500曲を用いてこれらのクラスターを学習しない。
    代わりに、K-meansモデルを学習するために約200曲でこのデータベースからサンプリングする。
    次に実験用データベースの各楽曲を以下の手順でN次元ベクトルに変換する。
    従来と同様に各楽曲を一連のMFCCベクトルに変換する。各ベクトルについてN個のクラスターそれぞれに対するスコアを算出し、
    最高スコアのクラスターのカウンタをインクリメントする。正規化されたカウント値の集合がその楽曲のN次元表現となる。
    """