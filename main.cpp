#include <iostream>
#include <memory>
#include <windows.h>

#include "SceneManager.h"

using namespace std;

// main()은 앱을 초기화하고 실행시키는 기능만 합니다.
// 콘솔창이 있으면 디버깅에 편리합니다.
// 디버깅할 때 애매한 값들을 cout으로 출력해서 확인해보세요.
int main() {
    SceneManager m_sceneManager;

    if (!m_sceneManager.Initialize()) {
        cout << "Initialization failed." << endl;
        return -1;
    }

    return m_sceneManager.Run();
}
