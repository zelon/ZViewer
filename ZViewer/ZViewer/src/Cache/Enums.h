#pragma once

/// 캐시를 어느 방향부터 먼저할 것인지, 우선시할 것인지를 위해...
/**
 예를 들어 사용자가 PageDown 으로 다음 이미지들을 보고 있다면 다음 이미지들을 좀 더 캐쉬해놓고,
 PageUp 으로 이전 이미지 방향으로 보고 있다면 이전 이미지들을 좀 더 캐쉬해놓기 위해서이다.
*/
enum class ViewDirection {
	kForward,	///< PageDown 등으로 다음 파일을 보았다
	kBackward,	///< PageUp 등으로 이전 파일을 보았다.
};

enum class RequestType {
	kCurrent,
	kPreCache
};
