#pragma once

class CTMat;
class CVec;

namespace dlgproceditops {
CVec GetMirrorScale();
CVec GetInvertedScale();
CVec GetRotAng();
CTMat GetInvertedRotTrnMat();
CTMat GetRotTrnMat();
CVec GetScale();
void SetMirrorScale(double = -1., double = 1., double = 1.);
void SetScale(double = 1., double = 1., double = 1.);
void SetRotAng(double = 0., double = 0., double = 45.);
void SetRotOrd(int = 0, int = 1, int = 2);
}  // namespace dlgproceditops
