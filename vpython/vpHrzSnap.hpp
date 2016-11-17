/* 
 * File:   vpHrzSnap.hpp
 * Author: ddoledec
 *
 * Created on April 15, 2015, 9:42 AM
 */

#ifndef VPHRZSNAP_HPP
#define	VPHRZSNAP_HPP

#include"vmSnapper.hpp"

class Volume;
class SurveyRegion;
class vgHorizon;


/**
 * Snap horizon to the seismic (pick, trough...)
 */
class vpHrzSnap {
public:

    enum Mode {
        POSITIVE_PEAK = 0, NEGATIVE_PEAK,
        POSITIVE_ZEROCROSS, NEGATIVE_ZEROCROSS,
        POSITIVE_MAXGRADIENT, NEGATIVE_MAXGRADIENT
    };

    vpHrzSnap();
    virtual ~vpHrzSnap();
    static void exposePython();

    bool snap(const Volume& vol, vgHorizon& hrz, int idx);

    void setRegion(SurveyRegion* rg);
    void clearRegion();

    // open this to  the user side

    void setWindowSize(int size) {
        windowSize = size;
    }

    void setMode(Mode mode) {
        snapMode = mode;
    }

protected:
    int windowSize;
    vpHrzSnap::Mode snapMode; // default set to POSITIVE_PEAK
    SurveyRegion* myRegion;
};

#endif	/* VPHRZSNAP_HPP */

