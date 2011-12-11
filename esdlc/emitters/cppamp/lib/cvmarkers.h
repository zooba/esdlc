// A stub file with no-op implementations of the Concurrency Visualiser functions.
//
// Replace this file with the one from an installation of Visual Studio 11.

const GUID CvDefaultProviderGuid = {0};
const int CvImportanceNormal = 0;

typedef struct { } *PCV_PROVIDER;
typedef struct { } *PCV_MARKERSERIES;
typedef struct { } *PCV_SPAN;

inline void CvInitProvider(const GUID*, PCV_PROVIDER*) { }
inline void CvReleaseProvider(PCV_PROVIDER) { }
inline void CvCreateMarkerSeriesW(PCV_PROVIDER, const wchar_t*, PCV_MARKERSERIES*) { }
inline void CvReleaseMarkerSeries(PCV_MARKERSERIES) { }
inline void CvEnterSpanW(PCV_MARKERSERIES, PCV_SPAN*, const wchar_t*, ...) { }
inline void CvLeaveSpan(PCV_SPAN) { }
inline void CvWriteMessageExW(PCV_MARKERSERIES, int, int, const wchar_t*, ...) { }
