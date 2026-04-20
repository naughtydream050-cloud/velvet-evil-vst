# QUAD-ENGINE ORCHESTRATION v1.0
> SSOT: D:\Development | 美学: 退廃・ダークコア・ハイファッション

## フェーズ1: UI/UX — Stitch ワークフロー

| ステップ | エンジン | 役割 |
|---|---|---|
| Stitch.Capture | Claude | 参照画像解析 → px座標・カラーコード・余白比率抽出 |
| Stitch.Draft | Qwen2.5-coder (3s timeout) | setBounds()/paint() 初期コードドラフト |
| Stitch.Integrate | Claude | FontOptions検査・JUCE7互換・PixelPerfect補正 → 確定版書込 |
| Stitch.Commit | Claude Code | pre-flight grep → git commit "UI: Stitch [Name]" → push |

## フェーズ2: DSP/ロジック設計

| ステップ | エンジン | 役割 |
|---|---|---|
| DSP.Spec | Claude | アルゴリズム要件を100字構造化 → Gemini外注チケット生成 |
| DSP.Research | Gemini 2.0 Flash | 数式・参考実装・注意点をMarkdownで返答 |
| DSP.Implement | Claude | Gemini回答をJUCE C++翻訳・APVTS・スレッドセーフ確認 |
| DSP.Verify | Phi-3.5 (8s timeout) | 論理一貫性レビュー（timeout → Claude visual check） |

### Gemini外注タイミング規則
- アルゴリズム名が既知（Tape Wobble LFO, Schroeder Reverb等）→ 外注する
- Claude単独で数式が不確かな場合 → 外注する
- 実装100行以下の自明なDSP → 外注しない（Claude直接実装）

## フェーズ3: コードレビュー & デバッグ

| ステップ | エンジン | 役割 |
|---|---|---|
| Review.Static | Claude Code | `Select-String -Pattern "FontOptions\|new.*Editor\|delete.*Editor"` 既知地雷検出 |
| Review.Logic | Phi-3.5 (8s timeout) | diff論理エラー・null参照・スレッド安全性チェック |
| Review.Fallback | Claude | Phi timeout時 → diff目視レビュー + ECC Trust更新 |
| Debug.CI | Claude Code | Actions失敗時 → ログ末尾50行grep → 既知パターン照合・自動パッチ |

### Phi起動コマンド
```powershell
$job = Start-Job { ollama run phi3.5 "REVIEW: $args" }
$result = Wait-Job $job -Timeout 8 | Receive-Job
if (-not $result) { Stop-Job $job; Write-Host "PHI_TIMEOUT: Claude fallback" }
```

## フェーズ4: CI/CD & ドキュメント管理

| ステップ | エンジン | 役割 |
|---|---|---|
| CI.Push | Claude Code | push後 Invoke-RestMethod でrun取得（gh run watch禁止） |
| CI.Watch | Claude Code | 30秒間隔でstatus確認 → completed後にSTEP完了 |
| STATE.Update | Claude | STATE.md の ActiveProject/LastCommit/ActionsStatus 自動更新 |
| Blueprint.Sync | Claude | 工程完了時に blueprint.md ECC Trustスコア更新（新知見Trust 0.5登録） |

## 起動コマンド一覧
```
quad_boot.ps1 -Phase ui     → Stitch起動（Claude+Qwen）
quad_boot.ps1 -Phase dsp    → DSP設計（Claude+Gemini+Phi）
quad_boot.ps1 -Phase review → レビュー（Claude+Phi）
quad_boot.ps1 -Phase ci     → CI監視（Claude Code）
quad_boot.ps1               → 全エンジン疎通確認のみ
```

---
*ECC Trust: 知見スコア 0.1–1.0 | 更新: 工程完了時*

---

## オーケストレーション規約 v1.1（更新: 2026-04-20）
*Post-Mortem #01 の知見を反映*

### [絶対座標主義]
- 「中央付近」「だいたいここ」等の曖昧な座標指定を**禁止**する
- すべての配置は以下の数学的確定コードを優先すること:
  ```cpp
  // 推奨パターン
  auto bounds = getLocalBounds();
  comp.setBounds(bounds.withSizeKeepingCentre(w, h));
  comp.setCentrePosition(bounds.getCentreX(), targetY);
  ```
- ハードコード数値を使う場合は必ず Stitch 解析の px 値をコメントで併記すること:
  `wobbleKnob.setBounds(48, 118, 150, 150); // Stitch: center(123,193)`

### [アセット優先描画]
- `paint()` 内の手書きテクスチャ（亀裂・錆・グラデーション）を**禁止**する
- 背景・テクスチャ・アイコン類はすべて PNG → BinaryData に組み込み `drawImageAt()` で描画すること
- 実装順序: 「PNG 用意 → CMakeLists に juce_add_binary_data → BinaryData::xxx_png 参照」
- やむを得ず手書きする場合は 10 行以内に収め、コメントで「BinaryData 移行予定」を明記

### [リソース・ガードレール]
- コンピューター使用（screenshot / mouse click）を起動する前に、**必ず**以下を実行すること:
  1. 「現在のコードと理想画像の差分」をテキストで箇条書きリスト化
  2. 「残り修正ターン数: N」を自己宣言（N ≤ 2）
  3. 宣言なしの screenshot 実行は規約違反とし、オーケストレーターはそのターンを無効化する
- セッション生成は「1タスク = 1セッション」原則。同一目的での重複セッション生成を禁止する
