import os
import numpy as np
import pandas as pd
import librosa
import glob
import warnings

# librosaの警告を抑制（mp3読み込み時などに出ることがあるため）
warnings.filterwarnings('ignore')

# --- 設定 ---
# 音声ファイルがあるルートディレクトリ
INPUT_AUDIO_ROOT = "./downloaded_mp3/" 
# 結果の保存先
OUTPUT_CSV_PATH = "./mpeg7_features.csv"

class MPEG7Approximator:
    def __init__(self, sr=22050):
        self.sr = sr
        # MPEG-7推奨のフレームサイズに近い設定 (Window: ~30ms, Hop: 10ms)
        self.n_fft = 1024 
        self.hop_length = 512

    def extract_features(self, file_path):
        """
        1曲からMPEG-7近似特徴量（約80~90次元）を抽出する
        """
        try:
            # 音声ロード (y: 信号, sr: サンプリングレート)
            y, sr = librosa.load(file_path, sr=self.sr)
            
            # 無音区間や極端に短い曲の除外
            if len(y) < self.n_fft:
                return None

            features = {}

            # ==========================================
            # 1. Basic Spectral Descriptors (基本スペクトル)
            # ==========================================
            
            # (1) AudioPower -> RMS Energy (音の大きさ)
            rms = librosa.feature.rms(y=y, frame_length=self.n_fft, hop_length=self.hop_length)
            self._add_stats(features, "AudioPower", rms)

            # (2) AudioSpectrumCentroid (スペクトル重心 - 明るさ)
            centroid = librosa.feature.spectral_centroid(y=y, sr=sr, n_fft=self.n_fft, hop_length=self.hop_length)
            self._add_stats(features, "SpectrumCentroid", centroid)

            # (3) AudioSpectrumSpread (スペクトル広がり - 帯域幅)
            spread = librosa.feature.spectral_bandwidth(y=y, sr=sr, n_fft=self.n_fft, hop_length=self.hop_length)
            self._add_stats(features, "SpectrumSpread", spread)

            # (4) AudioSpectrumFlatness (スペクトル平坦度 - ノイズっぽさ)
            flatness = librosa.feature.spectral_flatness(y=y, n_fft=self.n_fft, hop_length=self.hop_length)
            self._add_stats(features, "SpectrumFlatness", flatness)
            
            # (5) AudioSpectrumRolloff (高周波成分の割合) - MPEG-7のEdgeに近い概念
            rolloff = librosa.feature.spectral_rolloff(y=y, sr=sr, n_fft=self.n_fft, hop_length=self.hop_length)
            self._add_stats(features, "SpectrumRolloff", rolloff)

            # ==========================================
            # 2. Signal Parameters (信号パラメータ)
            # ==========================================

            # (6) AudioFundamentalFrequency -> Zero Crossing Rate (基本周波数の簡易代替)
            zcr = librosa.feature.zero_crossing_rate(y, frame_length=self.n_fft, hop_length=self.hop_length)
            self._add_stats(features, "ZeroCrossingRate", zcr)

            # ==========================================
            # 3. Timbral Temporal / Spectral (音色)
            # ==========================================

            # (7) AudioSpectrumEnvelope -> Spectral Contrast (帯域ごとのコントラスト)
            # 7つの帯域における山と谷の差 (MPEG-7のエンベロープに近い情報を保持)
            contrast = librosa.feature.spectral_contrast(y=y, sr=sr, n_fft=self.n_fft, hop_length=self.hop_length)
            # contrastは (7バンド, 時間) の形状
            for i in range(contrast.shape[0]):
                self._add_stats(features, f"SpectrumContrast_Band{i}", contrast[i])

            # (8) AudioSpectrumBasisProjection -> MFCC (音色の主要成分)
            # MPEG-7では低次元投影を使いますが、一般的にMFCC 20次元で代用されます
            mfcc = librosa.feature.mfcc(y=y, sr=sr, n_mfcc=20, n_fft=self.n_fft, hop_length=self.hop_length)
            # 0次元目（パワー）を除く1~19次元を使用する場合が多いですが、ここでは全20次元取得
            for i in range(mfcc.shape[0]):
                self._add_stats(features, f"MFCC_{i}", mfcc[i])
            
            # ==========================================
            # 4. Harmonic (調波性 - オプション)
            # ==========================================
            # もし特徴量数が足りない場合は Chroma を追加します (12次元)
            # MPEG-7の AudioHarmonicity の代用
            chroma = librosa.feature.chroma_stft(y=y, sr=sr, n_fft=self.n_fft, hop_length=self.hop_length)
            for i in range(chroma.shape[0]):
                self._add_stats(features, f"Chroma_{i}", chroma[i])

            return features

        except Exception as e:
            print(f"Error processing {file_path}: {e}")
            return None

    def _add_stats(self, feature_dict, name, data):
        """
        Bag-of-Frameアプローチ:
        時系列データから統計量 (Mean, Std) を計算して辞書に追加する
        """
        # 平均 (Mean)
        feature_dict[f"{name}_Mean"] = np.mean(data)
        # 標準偏差 (Std)
        feature_dict[f"{name}_Std"] = np.std(data)
        
        # 先行研究に "sum" とあった場合、以下のように合計も追加可能
        # feature_dict[f"{name}_Sum"] = np.sum(data)

def main():
    print("MPEG-7 特徴量抽出を開始します...")
    
    extractor = MPEG7Approximator()
    all_data = []
    
    # mp3ファイルを再帰的に検索
    # 音声ファイルの拡張子に合わせて変更してください (*.mp3, *.wav など)
    search_path = os.path.join(INPUT_AUDIO_ROOT, "**/*.mp3")
    audio_files = glob.glob(search_path, recursive=True)
    
    if not audio_files:
        print(f"エラー: ディレクトリ '{INPUT_AUDIO_ROOT}' に音声ファイルが見つかりません。")
        return

    print(f"対象ファイル数: {len(audio_files)}件")

    for i, file_path in enumerate(audio_files):
        # ファイル名からアーティスト名・曲名を取得 (ファイル名が "Artist_Track.mp3" の場合)
        filename = os.path.basename(file_path)
        name_body = os.path.splitext(filename)[0]
        try:
            artist, track = name_body.split('_', 1)
        except ValueError:
            artist, track = "Unknown", name_body

        print(f"[{i+1}/{len(audio_files)}] Processing: {filename}")
        
        # 特徴量抽出
        features = extractor.extract_features(file_path)
        
        if features:
            # メタデータを追加
            features["Artist"] = artist
            features["Track"] = track
            features["Filename"] = filename
            all_data.append(features)

    # DataFrameに変換してCSV保存
    if all_data:
        df = pd.DataFrame(all_data)
        
        # カラムの順序を整理 (Artist, Trackを先頭に)
        cols = ["Artist", "Track", "Filename"] + [c for c in df.columns if c not in ["Artist", "Track", "Filename"]]
        df = df[cols]
        
        df.to_csv(OUTPUT_CSV_PATH, index=False, encoding='utf-8-sig')
        print(f"\n✅ 完了: '{OUTPUT_CSV_PATH}' に保存しました。")
        print(f"抽出された特徴量の数（次元数）: {len(cols) - 3}") # メタデータ分を引く
    else:
        print("特徴量が抽出できませんでした。")

if __name__ == "__main__":
    main()