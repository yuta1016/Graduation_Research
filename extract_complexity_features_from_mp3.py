import numpy as np
import librosa
from scipy.spatial.distance import jensenshannon
from scipy.special import softmax

class MusicComplexityFeatures:
    def __init__(self, file_path, sr=44100):
        self.file_path = file_path
        self.sr = sr
        # 2.97秒のウィンドウ、1秒ごとの移動 (論文記述に基づく)
        self.n_fft = int(2.97 * sr) 
        self.hop_length = int(1.0 * sr)
        
        # 音声のロード
        self.y, _ = librosa.load(file_path, sr=sr)
        
        # 特徴量のキャッシュ
        self.chroma_seq = None
        self.timbre_seq = None
        self.rhythm_seq = None
        self.arousal_mean = None
        self.arousal_std = None

    def _normalize_distribution(self, vector):
        """
        ベクトルを確率分布（合計1, 正の値）に変換する。
        MFCCなど負の値を含むものはSoftmaxを使用。
        """
        # 値が全て0の場合は均等分布を返す
        if np.sum(np.abs(vector)) == 0:
            return np.ones(len(vector)) / len(vector)
        
        # Softmaxを使用して負の値を考慮しつつ正規化
        return softmax(vector)

    def extract_chroma(self):
        """
        Chroma特徴量の抽出 (12次元)
        論文[30]のNNLSに近い挙動として、CQTベースのクロマを使用し確率分布化する。
        """
        # 時間分解能を論文の "0.25 sec" に合わせるためのhop_length
        hop_length_chroma = int(0.25 * self.sr)
        
        # CQTクロマの計算
        chroma = librosa.feature.chroma_cqt(y=self.y, sr=self.sr, hop_length=hop_length_chroma)
        
        # 各フレームを確率分布として正規化 (列ごとに合計1にする)
        # axis=0は特徴量次元(12音階)
        self.chroma_seq = librosa.util.normalize(chroma, norm=1, axis=0).T # (Time, 12)
        return self.chroma_seq

    def extract_timbre(self):
        """
        Timbre (MFCC) 特徴量の抽出 (36次元)
        """
        timbre_list = []
        n_samples = len(self.y)
        
        # 1秒ごとにループ (hop_length)
        for start in range(0, n_samples - self.n_fft, self.hop_length):
            end = start + self.n_fft
            segment = self.y[start:end]
            
            # MFCC計算
            mfcc = librosa.feature.mfcc(y=segment, sr=self.sr, n_mfcc=36)
            
            # (36, Time) -> 時間方向に平均して (36,) のベクトルにする
            mfcc_mean = np.mean(mfcc, axis=1)
            timbre_list.append(mfcc_mean)
            
        self.timbre_seq = np.array(timbre_list) # (Time, 36)
        
        # ★★★ 修正: 全体に対して標準化を行う (平均0, 分散1) ★★★
        # これにより、値が大きすぎてSoftmaxでinfになるのを防ぐ
        if np.std(self.timbre_seq) > 0:
            self.timbre_seq = (self.timbre_seq - np.mean(self.timbre_seq)) / np.std(self.timbre_seq)
            
        return self.timbre_seq

    def extract_rhythm(self):
        """
        Rhythm (Fluctuation Pattern近似) の抽出
        厳密なFPの実装は複雑なため、変調スペクトル（Modulation Spectrum）の概念で近似する。
        """
        # 1. メルスペクトログラムを計算
        S = librosa.feature.melspectrogram(y=self.y, sr=self.sr, n_fft=2048, hop_length=512)
        
        # 2. 2.97秒ごとのセグメントに分割してFFTをかける（リズムの周期性を探る）
        # ここでは簡易的に、librosaのtempogram（テンポグラム）の平均を使用するアプローチをとる
        # tempogramはリズムの周期性を表す特徴量 (384次元などになる)
        
        # 論文のウィンドウサイズに合わせてOnsetsを取得
        onset_env = librosa.onset.onset_strength(y=self.y, sr=self.sr)
        
        # テンポグラムを計算 (Time, 384)
        # win_lengthを論文の2.97秒に合わせて調整
        win_length_frames = int(2.97 * self.sr / 512)
        tempogram = librosa.feature.tempogram(onset_envelope=onset_env, sr=self.sr, 
                                              hop_length=int(1.0*self.sr/512), 
                                              win_length=win_length_frames)
        
        self.rhythm_seq = tempogram.T # (Time, Features)
        return self.rhythm_seq

    def extract_arousal(self):
        """
        Arousal (Loudness) の抽出
        Short-time magnitude (絶対値の和) の平均と標準偏差
        """
        # フレームごとの絶対値の和を計算
        # 論文: "2.97-second long Hamming window moving one second at a time"
        
        # 信号をフレーム化
        frames = librosa.util.frame(self.y, frame_length=self.n_fft, hop_length=self.hop_length)
        
        # Hamming窓を適用
        window = np.hamming(self.n_fft)
        window = window.reshape(-1, 1) # 放送用
        
        windowed_frames = frames * window
        
        # "sum of the absolute signal values"
        magnitudes = np.sum(np.abs(windowed_frames), axis=0)
        
        self.arousal_mean = np.mean(magnitudes)
        self.arousal_std = np.std(magnitudes)
        
        return {
            "ArousalMean": self.arousal_mean,
            "ArousalStd": self.arousal_std
        }

    def calculate_sc(self, component_seq, component_type):
        """
        Structural Change (SC) の計算
        修正: 値の爆発を防ぐため、np.sum ではなく np.mean を使用する
        """
        n_segments = component_seq.shape[0]
        sc_values = {}

        if component_type == 'chroma':
            js = range(3, 9) 
        else:
            js = range(1, 7) 

        for j in js:
            w_j = 2**(j - 1) 
            
            jsd_list = []
            
            for i in range(w_j, n_segments - w_j):
                # ★★★ 修正箇所: np.sum を np.mean に変更 ★★★
                # 合計だとウィンドウが長い(Timbre5,6)ときに値が大きくなりすぎてinfになるため
                vec_past = np.mean(component_seq[i - w_j : i], axis=0)
                vec_future = np.mean(component_seq[i : i + w_j], axis=0)
                
                # 正規化（Softmax）
                dist_past = self._normalize_distribution(vec_past)
                dist_future = self._normalize_distribution(vec_future)
                
                # JSD計算
                jsd = jensenshannon(dist_past, dist_future)
                
                # NaN / Inf 対策 (念のため)
                if not np.isfinite(jsd): jsd = 0.0
                
                jsd_list.append(jsd)
            
            if len(jsd_list) > 0:
                sc_mean = np.mean(jsd_list)
            else:
                sc_mean = 0.0
                
            sc_values[f"{component_type.capitalize()}{j}"] = sc_mean
            
        return sc_values

    def get_all_features(self):
        # 1. 各コンポーネントの時系列データを抽出
        print("Extracting Chroma...")
        self.extract_chroma()
        print("Extracting Timbre...")
        self.extract_timbre()
        print("Extracting Rhythm...")
        self.extract_rhythm()
        
        # 2. Arousalの計算
        print("Calculating Arousal...")
        arousal_feats = self.extract_arousal()
        
        # 3. Structural Change (SC) の計算
        print("Calculating Structural Change (SC)...")
        features = {}
        
        # Chroma (j=3..8)
        sc_chroma = self.calculate_sc(self.chroma_seq, 'chroma')
        features.update(sc_chroma)
        
        # Rhythm (j=1..6)
        sc_rhythm = self.calculate_sc(self.rhythm_seq, 'rhythm')
        features.update(sc_rhythm)
        
        # Timbre (j=1..6)
        sc_timbre = self.calculate_sc(self.timbre_seq, 'timbre')
        features.update(sc_timbre)
        
        # Arousalを追加
        features.update(arousal_feats)
        
        return features

# --- 実行例 ---
if __name__ == "__main__":
    # 音楽ファイルのパスを指定してください
    audio_file = "./downloaded_mp3/星野源_ドラえもん.mp3" 
    
    # ファイルが存在するか確認用ダミーチェック (実際には外してください)
    import os
    if os.path.exists(audio_file):
        extractor = MusicComplexityFeatures(audio_file)
        features = extractor.get_all_features()
        
        print("\n--- Calculated Features ---")
        for k, v in features.items():
            print(f"{k}: {v}")
    else:
        print("音楽ファイルを指定してください。")