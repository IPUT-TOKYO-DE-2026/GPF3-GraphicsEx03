#include "FrameBufferEmulator.h"

int p;
int x;
int y;
int xMin;
int yMin;
int xMax;
int yMax;
int f;  // 拡大のステップ幅
int f1; // 1つ前の拡大ステップ幅
int f2; // 2つ前の拡大ステップ幅

const int bgr[3][3] = {
	{   0,   0, 255 },
	{   0, 255, 255 },
	{ 255,   0,   0}
};

int centerX; // 円の中心座標X
int centerY; // 円の中心座標Y
int radius;  // 円の半径

// 初期化処理（最初に1回だけ呼び出される）
void FrameBufferEmulator::initUser()
{
	p = 0;
	// 画面の中心を求める
	x = width / 2;
	y = height / 2;
	// 探索領域の境界を初期化
	xMin = x;
	yMin = y;
	xMax = x;
	yMax = y;
	f = 1;  // 拡大のステップ幅
	f1 = 1; // 1つ前の拡大ステップ幅
	f2 = 0; // 2つ前の拡大ステップ幅
	dontClear = true; // 毎フレームクリアせずに描画する
}

// 描画処理（毎フレーム呼び出される）
void FrameBufferEmulator::drawUser(unsigned char* buff, int mode, int keyLevel, int keyTrigger)
{
	if (keyTrigger == SDLK_RIGHT) { // 右矢印キーが押されたらステップを進める
		if (xMin < 0 && xMax >= width && yMin < 0 && yMax >= height) {
			return;
		}
		p++;

		int colorNo = p % 3;
		f = f1 + f2;  // 次の拡大ステップ幅を計算(1つ前と2つ前の値の和)

		int sx, sy, ex, ey;
		// ステップ p に応じて螺旋状に領域を拡大
		if (p % 4 == 0) { // ステップ 0, 4, 8, ... (東方向へ拡大)
			xMax += f;
			ex = xMax;
			sx = ex - f;
			ey = yMax;
			sy = yMin;
		}
		else if (p % 4 == 1) { // ステップ 1, 5, 9, ... (北方向へ拡大)
			yMin -= f;
			ex = xMax;
			sx = xMin;
			sy = yMin;
			ey = sy + f;
		}
		else if (p % 4 == 2) { // ステップ 2, 6, 10, ... (西方向へ拡大)
			xMin -= f;
			sx = xMin;
			ex = sx + f;
			ey = yMax;
			sy = yMin;
		}
		else { // ステップ 3, 7, 11, ... (南方向へ拡大)
			yMax += f;
			ex = xMax;
			sx = xMin;
			ey = yMax;
			sy = ey - f;
		}
		if (sy < 0) { sy = 0; }
		if (ey >= height) { ey = height; }
		if (sx < 0) { sx = 0; }
		if (ex >= width) { ex = width; }

		// 次の拡大ステップ幅計算のために値を更新
		f2 = f1;
		f1 = f;

		// sx, sy, ex, eyで指定された範囲を塗りつぶす
		unsigned char* pixel;
		for (int i = sy; i < ey; i++) {
			pixel = buff + i * width * 3 + sx * 3;
			for (int j = sx; j < ex; j++) {
				*pixel++ = bgr[colorNo][0]; // B
				*pixel++ = bgr[colorNo][1]; // G
				*pixel++ = bgr[colorNo][2]; // R
			}
		}
	}
}
