/* 
 * File:   vpCaptureImage.hpp
 * Author: ddoledec
 *
 * Created on January 12, 2016, 2:52 PM
 */

#ifndef VPCAPTUREIMAGE_HPP
#define	VPCAPTUREIMAGE_HPP

/**
 * Define interfaces to capture images from an active torando session
 */
class vpCaptureImage {
public:
    vpCaptureImage();
    vpCaptureImage(const vpCaptureImage& orig);
    virtual ~vpCaptureImage();    
       
    static void exposePython();    
    
private:

};

#endif	/* VPCAPTUREIMAGE_HPP */

