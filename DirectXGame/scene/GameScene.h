#pragma once

#include "Audio.h"
#include "DebugText.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "time.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();
	// タイトル(スプライト)
	uint32_t textureHandleTitle_ = 0;
	Sprite* spriteTitle_ = nullptr;

	uint32_t textureHandleGameOver_ = 0;
	Sprite* spriteGameOver_ = nullptr;

	uint32_t textureHandleGameClear_ = 0;

	// BG(スプライト)
	uint32_t textureHandleBG_ = 0;
	Sprite* spriteBG_ = nullptr;

	uint32_t textureHandleEnter_ = 0;
	Sprite* spriteEnter_ = nullptr;

	//ビュープロジェクション(共通)
	ViewProjection viewProjection_;

	//ステージ
	uint32_t textureHandleStage_ = 0;
	Model* modelStage_ = nullptr;
	WorldTransform worldTransformStage_[20];

	//プレイヤー
	uint32_t textureHandlePlayer_ = 0;
	Model* modelPlayer_ = nullptr;
	WorldTransform worldTransformPlayer_;

	void PlayerUpdate();	//プレイヤーの更新

	//ビーム
	uint32_t textureHandleBeam_ = 0;
	Model* modelBeam_ = nullptr;
	WorldTransform worldTransformBeam_[10];

	// スコア数値(スプライト)
	uint32_t textureHandleNumber_ = 0;
	Sprite* spriteNumber_[5] = {};

	uint32_t textureHandleScore_ = 0;
	Sprite* spriteScore_ = nullptr;

	// ライフ(スプライト)
	uint32_t textureHandleLife_ = 0;
	Sprite* spriteLife_[3] = {};

	void BeamUpdate();		//ビーム更新

	void BeamMove();		//ビーム移動

	void BeamBorn();		//ビーム発生

	int beamFlag_[10] = {}; // ビーム存在フラグ(0:存在しない 1:存在する)

	//敵
	uint32_t textureHandleEnemy_ = 0;
	Model* modelEnemy_ = nullptr;
	WorldTransform worldTransformEnemy_[10];

	void EnemyUpdate();		//敵更新

	void EnemyMove();		//敵移動

	void EnemyBorn();		//敵発生

	void EnemyJump();

	int enemyFlag_[10] = {};

	//衝突判定
	void Collision();			//衝突判定
	void CollisionPlayerEnemy();//衝突判定(プレイヤーと敵)

	void CollisionBeamEnemy();	//衝突判定(ビームと敵)

	//サウンド
	uint32_t soundDataHandleTitleBGM_ = 0;		// タイトルBGM
	uint32_t soundDataHandleGamePlayBGM_ = 0;	// ゲームプレイBGM
	uint32_t soundDataHandleGameOverBGM_ = 0;	// ゲームオーバーBGM
	uint32_t soundDataHandleGameClearBGM_ = 0;   // ゲームクリアBGM
	uint32_t soundDataHandleEnemyHitSE_ = 0;	// 敵ヒットSE
	uint32_t soundDataHandlePlayerHitSE_ = 0;	//プレイヤーヒットSE
	uint32_t voiceHandleBGM_ = 0;				//音声再生ハンドル
	
	//ステージ
	void StageUpdate();

	DebugText* debugText_ = nullptr;

	int gameScore_ = 0;	//ゲームスコア

	int playerLife_ = 3;// プレイヤーライフ

	int gameTimer_ = 0;

	int beamTimer_ = 0;

	int playerTimer_ = 0;

	float enemySpeed_[10] = {};	//スピード

	float enemyJumpSpeed_[10] = {};	//敵ジャンプの移動速度

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	void GamePlayUpdate();		//ゲームプレイ更新
	void GamePlayDraw3D();		//ゲームプレイ3D描画
	void GamePlayDraw2DBack();	//ゲームプレイ2D背景描画
	void GamePlayDraw2DNear();	//ゲームプレイ2D近景描画
	void GamePlayStart();		//ゲームプレイ初期化

	void TitleUpdate();		//タイトル更新
	void TitleDraw2DNear();	//タイトル2D

	void GameOverUpdate();
	void GameOverDraw2DNear();

	void GameClearUpdate();
	void GameClearDraw2DNear();

	void DrawScore();
	void DrawLife();

	int sceneMode_ = 1;
};