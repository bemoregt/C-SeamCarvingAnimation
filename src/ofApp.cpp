#include "ofApp.h"

// 에너지 맵 계산 -------------------------------------
vector<vector<float>> ofApp::calculateEnergy(const ofImage& img) {
    int width = img.getWidth();
    int height = img.getHeight();
    vector<vector<float>> energy(height, vector<float>(width));
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // 이웃 픽셀과의 색상 차이를 계산
            ofColor left = (x > 0) ? img.getColor(x-1, y) : img.getColor(x, y);
            ofColor right = (x < width-1) ? img.getColor(x+1, y) : img.getColor(x, y);
            ofColor up = (y > 0) ? img.getColor(x, y-1) : img.getColor(x, y);
            ofColor down = (y < height-1) ? img.getColor(x, y+1) : img.getColor(x, y);
            
            float dx = abs(left.r - right.r) + abs(left.g - right.g) + abs(left.b - right.b);
            float dy = abs(up.r - down.r) + abs(up.g - down.g) + abs(up.b - down.b);
            
            energy[y][x] = dx + dy;
        }
    }
    return energy;
}

// 수직 심 찾기 ----------------------------------------------
vector<int> ofApp::findVerticalSeam(const vector<vector<float>>& energy) {
    int height = energy.size();
    int width = energy[0].size();
    
    vector<vector<float>> dp = energy;
    vector<vector<int>> backtrack(height, vector<int>(width));
    
    // DP를 사용하여 최소 에너지 경로 계산
    for (int y = 1; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float minEnergy = dp[y-1][x];
            int minX = x;
            
            if (x > 0 && dp[y-1][x-1] < minEnergy) {
                minEnergy = dp[y-1][x-1];
                minX = x-1;
            }
            if (x < width-1 && dp[y-1][x+1] < minEnergy) {
                minEnergy = dp[y-1][x+1];
                minX = x+1;
            }
            
            dp[y][x] += minEnergy;
            backtrack[y][x] = minX;
        }
    }
    
    // 최소 에너지 경로 추적
    vector<int> seam(height);
    int minX = 0;
    float minEnergy = dp[height-1][0];
    
    for (int x = 1; x < width; x++) {
        if (dp[height-1][x] < minEnergy) {
            minEnergy = dp[height-1][x];
            minX = x;
        }
    }
    
    seam[height-1] = minX;
    for (int y = height-2; y >= 0; y--) {
        seam[y] = backtrack[y+1][seam[y+1]];
    }
    
    return seam;
}

// 수직 심 제거 ---------------------------------------------
void ofApp::removeVerticalSeam(ofImage& img, const vector<int>& seam) {
    int width = img.getWidth();
    int height = img.getHeight();
    
    ofImage newImg;
    newImg.allocate(width-1, height, OF_IMAGE_COLOR);
    
    for (int y = 0; y < height; y++) {
        int seamX = seam[y];
        for (int x = 0; x < width-1; x++) {
            if (x < seamX) {
                newImg.setColor(x, y, img.getColor(x, y));
            } else {
                newImg.setColor(x, y, img.getColor(x+1, y));
            }
        }
    }
    
    img = newImg;
}

// Seam Carving으로 이미지 크기 조절 -----------------------------
void ofApp::resizeWithSeamCarving(ofImage& img, int targetWidth, int targetHeight) {
    while (img.getWidth() > targetWidth) {
        vector<vector<float>> energy = calculateEnergy(img);
        vector<int> seam = findVerticalSeam(energy);
        removeVerticalSeam(img, seam);
    }
    
    // 회전하여 높이도 조절
    img.rotate90(1);
    while (img.getWidth() > targetHeight) {
        vector<vector<float>> energy = calculateEnergy(img);
        vector<int> seam = findVerticalSeam(energy);
        removeVerticalSeam(img, seam);
    }
    img.rotate90(-1);
}

// 이미지를 창의 중앙에 배치하는 함수 ------------------------------
void ofApp::centerImage() {
    if (imageLoaded) {
        float windowWidth = ofGetWidth();
        float windowHeight = ofGetHeight();
        float imageWidth = workingImage.getWidth();
        float imageHeight = workingImage.getHeight();
        
        // 원본 이미지 크기 정보 저장
        imageSizeInfo = "Image Size: " +
                       ofToString((int)originalImage.getWidth()) + " x " +
                       ofToString((int)originalImage.getHeight()) + " px";
        
        // 이미지가 창보다 큰 경우 크기를 절반으로 조정
        if (imageWidth > windowWidth || imageHeight > windowHeight) {
            imageWidth *= 0.5;
            imageHeight *= 0.5;
            workingImage.resize(imageWidth, imageHeight);
        }
        
        // 테두리가 보이도록 여백 추가 (각 방향 20픽셀)
        const float margin = 20;
        imageWidth = min(imageWidth, windowWidth - margin * 2);
        imageHeight = min(imageHeight, windowHeight - margin * 2);
        
        // 중앙 위치 계산 (여백 포함)
        float x = (windowWidth - imageWidth) / 2;
        float y = (windowHeight - imageHeight) / 2;
        
        resizeHandle.set(x, y, imageWidth, imageHeight);
    }
}

//==============================================================
//--------------------------------------------------------------
void ofApp::setup() {
    imageLoaded = false;
    isDragging = false;
    minWidth = 100;
    minHeight = 100;
    messageAlpha = 0;
    isHovering = false;
}

//--------------------------------------------------------------
// ofApp.cpp의 update() 함수에서 커서 변경 부분을 다음과 같이 수정
void ofApp::update() {
    if (isDragging) {
        resizeHandle.width = max(minWidth, (int)resizeHandle.width);
        resizeHandle.height = max(minHeight, (int)resizeHandle.height);
    }
    
    // 메시지 페이드 아웃 효과
    if (messageAlpha > 0) {
        messageAlpha -= 0.5;
    }
    
    // 마우스 호버링 체크
    if (imageLoaded) {
        glm::vec2 mouse(ofGetMouseX(), ofGetMouseY());
        float border = 10; // 테두리 감지 영역
        
        bool onBorder =
            (abs(mouse.x - resizeHandle.x) < border && mouse.y >= resizeHandle.y && mouse.y <= resizeHandle.y + resizeHandle.height) ||
            (abs(mouse.x - (resizeHandle.x + resizeHandle.width)) < border && mouse.y >= resizeHandle.y && mouse.y <= resizeHandle.y + resizeHandle.height) ||
            (abs(mouse.y - resizeHandle.y) < border && mouse.x >= resizeHandle.x && mouse.x <= resizeHandle.x + resizeHandle.width) ||
            (abs(mouse.y - (resizeHandle.y + resizeHandle.height)) < border && mouse.x >= resizeHandle.x && mouse.x <= resizeHandle.x + resizeHandle.width);
        
        isHovering = onBorder;
        
        // 마우스 커서 변경
    }
}


//--------------------------------------------------------------
void ofApp::draw() {
    if (imageLoaded) {
        workingImage.draw(resizeHandle);
        
        // 더 두꺼운 빨간색 테두리 그리기
        ofNoFill();
        ofSetColor(255, 0, 0);
        ofSetLineWidth(5);
        ofDrawRectangle(resizeHandle);
        
        // 이미지 크기 정보 표시 (왼쪽 상단 구석)
        ofSetColor(255);
        ofDrawBitmapString(imageSizeInfo, 10, 20);
        
        // 상태 메시지 표시
        if (messageAlpha > 0) {
            ofSetColor(255, 255, 255, messageAlpha);
            ofDrawBitmapString(statusMessage,
                             ofGetWidth()/2 - 100,
                             ofGetHeight() - 50);
        }
    }
}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
    if (isDragging) {
        float deltaX = x - dragStart.x;
        float deltaY = y - dragStart.y;
        
        resizeHandle.width += deltaX;
        resizeHandle.height += deltaY;
        
        dragStart = glm::vec2(x, y);
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    if (imageLoaded && resizeHandle.inside(x, y)) {
        isDragging = true;
        dragStart = glm::vec2(x, y);
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
    if (isDragging) {
        isDragging = false;
        
        // Seam Carving 적용
        resizeWithSeamCarving(workingImage,
                            resizeHandle.width,
                            resizeHandle.height);
        
        // 결과 저장 및 메시지 표시
        workingImage.save("resized_image.png");
        statusMessage = "이미지가 성공적으로 저장되었습니다: resized_image.png";
        messageAlpha = 255;
        
        // 이미지 크기 정보는 원본 크기로 유지
        // imageSizeInfo는 변경하지 않음
    }
}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
// 창 크기가 변경될 때 이미지 재중앙 정렬
void ofApp::windowResized(int w, int h) {
    centerImage();
}


//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {
    if (dragInfo.files.size() > 0) {
        originalImage.load(dragInfo.files[0]);
        workingImage = originalImage;
        imageLoaded = true;
        
        // 이미지 크기 조정 및 중앙 정렬
        centerImage();
    }
}