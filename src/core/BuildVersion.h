#pragma once

// Primary source of truth is the repo root VERSION file.
// CMake injects MW_BUILD_VERSION from that file for native builds.
#ifndef MW_BUILD_VERSION
#define MW_BUILD_VERSION "0.1"
#endif

namespace buildinfo {
inline constexpr const char* kVersion = MW_BUILD_VERSION;
inline constexpr const char* kDeveloper = "IlyaBOT";
inline constexpr const char* kDeveloperCompany = "IB-WorkShop";
inline constexpr const char* kDevelopmentYear = "2026";
}
