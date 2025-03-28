#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

	public:
		void setup() override;
		void update() override;
		void draw() override;
		void exit() override;

		void keyPressed(int key) override;
		void keyReleased(int key) override;
		void mouseMoved(int x, int y ) override;
		void mouseDragged(int x, int y, int button) override;
		void mousePressed(int x, int y, int button) override;
		void mouseReleased(int x, int y, int button) override;
		void mouseScrolled(int x, int y, float scrollX, float scrollY) override;
		void mouseEntered(int x, int y) override;
		void mouseExited(int x, int y) override;
		void windowResized(int w, int h) override;
		void dragEvent(ofDragInfo dragInfo) override;
		void gotMessage(ofMessage msg) override;
		
    // 이미지 처리 관련 함수들
    void centerImage();
    vector<vector<float>> calculateEnergy(const ofImage& img);
    vector<int> findVerticalSeam(const vector<vector<float>>& energy);
    void removeVerticalSeam(ofImage& img, const vector<int>& seam);
    void resizeWithSeamCarving(ofImage& img, int targetWidth, int targetHeight);
    
private:
    ofImage originalImage;    // 원본 이미지
    ofImage workingImage;     // 작업용 이미지
    ofRectangle resizeHandle; // 이미지 크기 조절 핸들
    
    // 상태 변수들
    bool isDragging;          // 드래그 중인지 여부
    bool imageLoaded;         // 이미지가 로드되었는지 여부
    bool isHovering;          // 마우스가 테두리 위에 있는지 여부
    
    // 크기 제한
    int minWidth;            // 최소 너비
    int minHeight;           // 최소 높이
    
    // UI 관련 변수들
    string statusMessage;     // 상태 메시지
    string imageSizeInfo;     // 이미지 크기 정보
    float messageAlpha;       // 메시지 투명도
    
    // 마우스 위치 저장
    glm::vec2 dragStart;     // 드래그 시작 위치
};