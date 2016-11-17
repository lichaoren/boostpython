


namespace vision {

    /**
     * \defgroup snapshot
     * 
     * snapshot is used to create images (png, jpeg..)
     * 
     * [snapshot is defined in vision package]
     * 
     * @warning
     * This module can only be imported and used under Tornado GUI mode (python dialog). Importing and using 
     *   this module under command line or torpy module will result in failure.
     * 
     * Below is an example to create images from the main window at diff IL/XL for two seismic/attributes dataset :
     * 
     * \code{.py}
           from base import Volume
           from vision import *
       
           #######  Create the parameters
           param = CaptureParameters()

           #######  File settings
           fileParameters = CaptureFileParameters()
           fileParameters.setPrefix("xxxx")
           # Output directory
           fileParameters.setPath("/tmp/")
           # Image format: png, jpg, png
           fileParameters.setFormat("png")
           # Name of the file according to the data & position
           # Available parameters:  Prefix, Suffix, GridNum, BookmarkName, SeismicName, 
           #                                          GatherName, WellName, AttrlName, WindowName
           fileParameters.setNaming(['Prefix','WindowName','SeismicName','AttrName','GridNum'])
       
           param.setFileParameters(fileParameters)
       
       
           #######  Data to capture (as a toggle list)
           dataParam = CaptureDataParameters()
       
           ### Add Seismic
           seismic1=vision.getSeismic('xp_M_aa.fdm')
           # set seismic1 at toggle position 0
           dataParam.addSeismic(seismic1,0)
           # set seismic1 at toggle position 1
           seismic2=vision.getSeismic('xp_M_a.fdm')
           dataParam.addSeismic(seismic2,1)
       
           ## Add Attribute
           attribute1=vision.getAttribute('xp_M_a3.fdm')
           dataParam.addAttribute(attribute1,0)
       
           attribute2=vision.getAttribute('xp_M_a2.fdm')
           dataParam.addAttribute(attribute2,1)
       
           param.setDataParameters(dataParam)
              
           #######  Location
           # Location from IL,XL
           volLocation=VolumeLocation()
           volLocation.setIL(20200)
           volLocation.setXL(41284, 41684, 64)
           param.setLocations(volLocation)
       

           # Main window
           captureImage(Window.MAIN_VIEW).capture(param)
     *
     * \endcode
     * 
     * @{
     */



    /**@}*/

}

