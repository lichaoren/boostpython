
#ifndef SHOTSEL_HPP
#define SHOTSEL_HPP


namespace algorithm {


    /**
     * @brief Ray-tracing(shotsel) 
     * 
     * In Tornado (Attribute->Analysis->3D Ray-Tracing)
     * 
     * 
     * Below is an example job to do shotsel:
     * \code{.py}
        from algorithm import ShotSel
        from base import *

        shotsel = ShotSel()
        shotsel.setMethod(0)
        shotsel.setVelocityPath("REMOTEFILE!houdat002:/data2/devtest/tornado/welltest/data/xp_M_a.fdm", "", "", "", "")
        shotsel.setRefHorizon("REMOTEFILE!houdat002:/data2/devtest/tornado/welltest/data/hrz_gb.hrz", "Herring")
        shotsel.setRayAngle(-40, 40, 10, -40, 40, 10, 0, 0, 1, 80)
        shotsel.setAcceptCompleteRays()
        shotsel.setShotDown()
        shotsel.setDepthDipAsSupplied(0, 0)
        shotsel.setMultiSourceSrcRecFile("REMOTEFILE!houdat002:/data2/devtest/tornado/thsu/shotsel/sourceReceiver2.txt", 10000);

        shotsel.setSummary("REMOTEFILE!houdat002:/data2/devtest/tornado/thsu/shotsel/output/test.sum", True, True, True);
        shotsel.setThreadNum(4);

        shotsel.start()
        shotsel.save()  
            

        */

    class ShotSel {
    public:

        /** 
         * @brief Specifing ray-tracing Method.
         *
         * @param mode Mode, ISO 0, VTI 1, TTI2  
         */
        void setMethod(int mode);

        /** 
         * @brief Setup velocities path.
         *
         * @param vel Velocity Path.  
         * @param delta Delta Path.
         * @param epilson Epislon Path.
         * @param theta Theta Path.
         * @param phi Phi Path. 
         */
        void setVelocityPath(string vel, string delta, string epilson, string theta, string phi);


        /** 
         * @brief Setup reflected horzion.
         *
         * @param hrzPath Horizon path.
         * @param hrzName Horizon name.
         */
         void setRefHorizon(string hrzPath, string hrzName);

        /** 
         * @brief Setup water bottom horzion.
         *
         * @param waterBottomHrzPath WaterBottom path.
         * @param hrzName Horizon name.
         */
         void setWaterBottom(string waterBottomHrzPath, string hrzName);

        /** 
         * @brief Setup ray angle(Incident wavefront angle) in degree.
         *
         * @param minX Minimum angle for X.  
         * @param maxX Maximum angle for X.
         * @param numX Number of rays in X direction.
         * @param minY Minimum angle for Y.  
         * @param maxY Maximum angle for Y.
         * @param numY Number of rays in Y direction.
         * @param minAzimuth Minimum angle for Azimuth.  
         * @param maxAzimuth Maximum angle for AzimuthY.
         * @param numAzimuth Number of rays in Azimuth direction.
         * @param MaxAngleNum Maxmimum number of rays.
         */
         void setRayAngle(float minX, float maxX, int numX, float minY, float maxY, int numY, float minAzimuth, float maxAzimuth, int numAzimuth, int MaxAngleNum);
        

        /** 
         * @brief To accept all rays.
         */
         void setAcceptAllRays();


        /** 
         * @brief To accept only the completed rays.
         */
         void setAcceptCompleteRays();


        /** 
         * @brief To accept the ray recored on surface line.
         *
         * @param depth Surface's depth.
         * @param receiverMode Mode for receiver. 0 = lines, 1 = region
         * @param firstX X Line start line number.
         * @param lastX X Line end line number.
         * @param IncX X direction increment.
         * @param firstY Y Line start line number.
         * @param lastY Y Line end line number.
         * @param IncY Y direction increment. 
         */
         void setAcceptRaysOnSurfaceLine(int depth, int receiverMode, int firstX, int lastX, int firstY, int lastY, int IncX, int IncY);


        /** 
         * @brief Setup shooting up.
         */
         void setShotUp();


        /** 
         * @brief Setup shooting up.
         */
         void setShotDown();

        /** 
         * @brief Setup depth and dip as supplied(Dip of up-going rays in degree).
         *
         * @param dipx Dip in X.
         * @param dipy Dip in Y.
         */
         void setDepthDipAsSupplied(int dipx, int dipy);


        /** 
         * @brief Setup depth and dip using horizon, need to load horizon via shotsel::setRefHorizon().
         *
         * @param dipHrz horizon name.
         */
         void setDepthDipAsHorizon(string dipHrz);


        /** 
         * @brief Setup sources.
         *
         * @param filePath Multi-sources file path.
         */
         void setMultiSourceFile(string filePath);


        /** 
         * @brief Specify sources using lines parameters.
         *
         * @param posX Source first X position.
         * @param posY Source first Y position.
         * @param posZ Source first Z position.
         * @param incX X Increment along line.
         * @param incY Y Increment along line.
         * @param numOfShot Shot Number along first X/Y direction.
         * @param SrcLineIncX X Increment between line.
         * @param SrcLineIncY Y Increment between line.
         * @param SrcLineNum Line Number along first X/Y direction.
         */
         void setMultiSourceInLine(float posX, float posY, float posZ, float incX, float incY, int numOfShot, int SrcLineIncX, int SrcLineIncY, int SrcLineNum);


        /** 
         * @brief Specify sources using directional lines parameters.
         *
         * @param posX Source first X position.
         * @param posY Source first Y position.
         * @param posZ Source first Z position.
         * @param dirLineSpace Line spacing.
         * @param dirShotSpace Shot spacing.
         * @param dirLineAngle Line angle in degree.
         */
         void setMultiSourceInDirLine(float posX, float posY, float posZ, float dirLineSpace, float dirShotSpace, float dirLineAngle);


        /** 
         * @brief Specify sources and receivers using seismic file(dataset).
         *
         * @param filePath Seismic file path(Dataset).
         * @param radius Receivers radius.
         * @param grpName Group name attribute specified in dataset.
         * @param shotX ShotX name attribute specified in dataset.
         * @param shotY ShotY name attribute specified in dataset.
         * @param shotZ ShotZ name attribute specified in dataset.
         * @param recX  RecX name attribute specified in dataset.
         * @param recY  RecY name attribute specified in dataset.
         * @param recZ  RecZ name attribute specified in dataset.
         */
         void setMultiSourceSeisFile(string filePath, float radius, string grpName, 
                                     string shotX, string shotY, string shotZ, 
                                     string recX, string recY, string recZ);


        /** 
         * @brief Specify sources and receivers using flat text file.
         *
         * @param filePath Text file path.
         * @param radius Receivers radius..
         */
         void setMultiSourceSrcRecFile(string filePath, float radius);


        /** 
         * @brief Specify rose diagram parsing parameters.
         *
         * @param filePath Output file path.
         * @param geom_xl Cross line geometry.
         * @param geom_sl In-line geometry.
         */
         void setRoseDiagram(string filePath, float geom_xl, float geom_sl);


        /** 
         * @brief Specify shot fold parsing parameters.
         *
         * @param filePath Output file path.
         * @param geom_xl Cross line geometry.
         * @param geom_sl In-line geometry.
         */       
         void setShotFold(string filePath, float geom_xl, float geom_sl);


        /** 
         * @brief Specify rays output file path.
         *
         * @param filePath Output file path.
         */ 
         void setRayPath(string filePath);


        /** 
         * @brief Setup summary output info.
         *
         * @param filePath Output file path.
         * @param angleInfo Boolean factor to specify whether to output reflector incident/azimuth angle.
         * @param saltTime Boolean factor to specify whether to output time info of salt and water bottom.
         * @param InitialAngle Boolean factor to specify whether to output initial incident azimuth angle.
         */ 
         void setSummary(string filePath, bool angleInfo, bool saltTime, bool InitialAngle);


        /** 
         * @brief Specify thread number.
         *
         * @param threadNum Thread number.
         */ 
         void setThreadNum(int threadNum);


        /** 
         * @brief Start the ray-tracing.
         */ 
         void start();

        /** 
         * @brief Save the summary and ray-path file..
         */ 
         void save();

        /** 
         * @brief Parse the result to generate rose diagram and shot fold.
         */ 
         void parse();

    };

}

#endif /* SHOTSEL_HPP */

