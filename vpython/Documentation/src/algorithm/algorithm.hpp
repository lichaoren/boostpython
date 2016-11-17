
/**
 * @namespace algorithm
 * 
 * This module will illustrate the algorithms from tornado python scripting interface
 * 
 * 
 * # Process on VOLUME
 * - Volume Regrid: VolumeRegrid
 * - Attribute Replace:  AttrReplace
 * - Unit Conversion: convertUnit
 * - Volume Math: VolumeMath
 * - Rca structure smooth: RcaSmooth
 * - Fdm Merge: MergeFdm
 * - Insert Salt: InsertSalt
 * - Volume Smooth: VolumeSmooth
 * 
 * # Process on HORIZON
 * - Horizon snap: HorizonSnap
 * - Horizon Extract Attribute : VolumeExtractAttribute
 * - Horizon Interpolation: GridInterp
 * - Horizon Smooth: HorizonSmooth
 * - Horizon Regrid: HorizonRegrid
 * - Horizon Resample: HorizonResample
 * - Trim Horizon: HorizonTrimmer
 * - Convert Horizon Domain: ConvertHrzDomain
 * 
 * @brief Algorithms available in tornado 
 */

namespace algorithm {

        /**  
         * Definition of the angle according to the unit Degrees/radians 
         * and the range [0,pi] or [-pi,+pi]
         * 
         * ### usage:
         * 
         * \code
         * AngleType.DegreeUnsigned
         * \endcode
         */
        enum AngleType {
            /// -180 to 180
        DegreeSigned, 
        /// 0 to 360
        DegreeUnsigned, 
        /// -pi to pi
        RadianSigned, 
        /// 0 to 2pi
        RadianUnsigned 
        };
}

