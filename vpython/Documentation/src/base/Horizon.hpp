#ifndef _Horizon_HPP_
#define _Horizon_HPP_


namespace base {

    /**
     * This class supports operations on the tornado horizon data
     * 
     * The horizon object is usually obtained from @ref HorizonGroup
     * 
     * 
     * Creating a Horizon object:
     * \code{.py}
     * 
     * from base import Horizon 
     * a = Horizon()
     * # or get horizon from HorizonGroup
     * hg = HorizonGroup()
     * hg.load('path')
     * hrz=hg.getHorizonByIndex(0)
     * #or using horizon name
     * hrz2 = hg.getHorizon('TOS1')
     * 
     * \endcode  
     * 
     * The example below shows how to use horizon and volume interfaces to implement user's own algorithm.
     *  In the example, we created a horizon attribute called TEST and store the volume average value 
     * within a window, the center of window is the horizon depth, to the new attribute
     * 
     * \code{.py}
     * 
       from base import *
       #load volume
       path='/data2/devtest/tornado/yanhliu/train_data/'
       vol=Volume()
       vol.load(path+'test1.fdm')
       #load horizon
       hg=HorizonGroup()
       hg.load(path+'test_hg.hrz')
       #create an attribte called test
       hg.addAttr('TEST')
       #look at TOS1
       hrz=hg.getHorizonByName('TOS1')
       #now we update the TEST attribute with some mean value around it
       hrz_size=hrz.getSize()
       vol_size=vol.getSize()
       for i in range(hrz_size[0]):
           for j in range(hrz_size[1]):
               #get the depth
               found, val=hrz.getAttrByGrid(i,j, 'DEPTH')
               if ( found==False):
                   continue
               #we need to convert to logical first
               log_hrz_pos=hrz.grd2log([i,j,0,0])
               #convert to volume grid, you have to be aware they might have
               #different geometry, that's why we convert it
               log_hrz_pos[2]=val
               volume_grd=map(int,vol.log2grd(log_hrz_pos))
               if (volume_grd[0]<0 or volume_grd[1]<0 or 
                   volume_grd[0]>=vol_size[0] or volume_grd[1]>=vol_size[1] ):
                   continue
               #assume we will not be out of boundary, but best to check
               val=0
               zind=volume_grd[2]
               for k in range(zind-2, zind+3):
                   volume_grd[2]=k
                   val=val+vol.getValue(volume_grd[0],volume_grd[1],volume_grd[2])
               hrz.setAttrByGrid(i,j,val/5.,'TEST')
       hg.save(True)

     * 
     * \endcode  
     * 
     * 
     * @warning for NumPy
     * Some methods need to have the package "numpy" available (this is specified in the description).   
     * For more information concerning numpy please, take the opportunity to visit the official page.
     * http://www.numpy.org/
     * This package is installed in most of the machines and you don't need to worry about it; but, if it is not 
     * available you need to install python (minimum release 2.7) and numpy on top of the
     * python installation. You will need also to set PYTHONHOME. 
         ie: 
            if python is installed in /myHome/python2.7 
            -> setenv PYTHONHOME /myHome/

    
     * 1) Basic arithmetic operators on arrays:
     * \code{.py}
        >>> a = np.array( [20,30,40,50] )
        >>> b = np.array( [0, 1, 2, 3])

        >>> c = a-b                      # Difference between 2 arrays
        >>> print (c)
        array([20, 29, 38, 47])
        >>> print (b**2)                 # Power 2
        array([0, 1, 4, 9])
        >>> print (10*np.sin(a))         # Trigonometric
        array([ 9.12945251, -9.88031624,  7.4511316 , -2.62374854])
        >>> print (a<35)                 # Create a boolean array from an inequality
        array([ True, True, False, False], dtype=bool)
     * \endcode  


    2) Matrix operation:

     * \code{.py}
        >>> A = np.array( [[1,1], [0,1]] )
        >>> B = np.array( [[2,0], [3,4]] )
        >>> print (A*B )                        # elementwise product
        array([[2, 0], [0, 4]])
        >>> print ((A.dot(B) )                  # matrix product
        array([[5, 4], [3, 4]])
        >>> print ((np.dot(A, B))               # another matrix product
        array([[5, 4], [3, 4]])
     * \endcode  

    3) Simple methods:

     * \code{.py}
        >>> a = np.random.random((2,3))
        >>> a
        array([[ 0.18626021,  0.34556073,  0.39676747],
               [ 0.53881673,  0.41919451,  0.6852195 ]])
        >>> a.sum()                            # Sum of all elements
        2.5718191614547998
        >>> a.min()                            # Minimum in the array
        0.1862602113776709
        >>> a.max()                            # Maximum in the array
        0.6852195003967595

        >>> a.sum(axis=0)                            # sum of each column
        >>> a.min(axis=1)                            # min of each row
        >>> a.cumsum(axis=1)                         # cumulative sum along each row
     * \endcode  


    And ... much more !
     */
    class Horizon {
    public:
        /** @brief horizon name*/
        string getName();
        
        void setName(string name);
        /** 
         * @brief the index of horizon attribute
         * @return index, -1 if no such an attribute
         */
        int getAttrIndex(string attr_name);
        
        /** @brief Attribute name */
        string getAttrName(int attr);


        /** @brief Number of attributes stored*/
        int getNumAttr() const;

        /**
         * @param hrz input horizon
         * @param x_log log x
         * @param y_log log y
         * @param attr_index attribute index
         * @return (true/false, val)
         */
        list borehole(double x_log, double y_log, int attr_index);

        /** Attribute value at given grid index
         * @return tuple the first return is whether the data exist, the second is the value if it exist
         */
        list<bool, float> getAttrByGrid(int xl, int sl, int attr_index);

        /** Attribute value at given grid index
         * @return tuple the first return is whether the data exist, the second is the value if it exist
         */
        tuple<bool, float> getAttrByGrid(int xl, int sl, string name);


        /** @brief Attribute value at given logical coordinate
         * @return tuple the first return is whether the data exist, the second is the value if it exist
         */
        tuple getAttrByLog(double x, double y, int attr_index);

        /** @brief Attribute value at given logical coordinate
         * @return tuple the first return is whether the data exist, the second is the value if it exist
         */
        tuple getAttrByLog(double x, double y, string name);

        /** Set attribute value at given grid index
         */
        void setAttrByGrid(int xl, int sl, double, int attr_index);
        /** Set attribute value at given grid index
         */ 
        void setAttrByGrid(int xl, int sl, double, string attr_name);
        /** Set attribute value at given logical coordinate
         */ 
        void setAttrByLog(double x, double y, double val, int attr_index);
        /** Set attribute value at given logical coordinate
         */ 
        void setAttrByLog(double x, double y, double val, string attr_name);


        /**
         * Origin of Horizon geometry
         * @return  list with 4 elements, the fourth element is dummy
         */
        list<double>[4] getOrigin();
        /**  @brief Step distance for x, y, z dimension @ref getInc() */
        list<double>[4] getStep();
        /** @brief grid size in x, y, z dimension */
        list<int>[4] getSize();
        /** @brief Minimum of crossline, inline, depth */
        list<int>[4] getMin();
        /** @brief Maximum of crossline, inline, depth */
        list<int>[4] getMax();
        /** @brief Increment of crossline, inline, depth */
        list<int>[4] getInc();

        /**
         * Copy from horizon 
         * @param source horizon
         * @param copy only data inside the region 
         * @warning The horizon must have the same geometry (no regrid is applied).
         */
        bool copy(Horizon source, vmMultiPoly2D  region);

        /** @brief  Transform grid location to survey coordinate */
        list<double>[4] grd2log(list<double>[4] v);
        
        /** @brief  Transform  survey coordinate to grid location */
        list<double>[4] grd2lin(list<double>[4] v);

        /** Transform grid location to world coordinates */
        list<double>[4] grd2wld(list<int>[4] grd);

        /**Get the attribute value at given grid
         */
        float getValue(int xl, int il, string attr_name);

        /**Get the attribute value at given grid */
        void setValue(int xl, int il, float v);

        /** Create a copy of the horizon (not added to the HorizonGroup).
         * It's rather a working copy to use as an intermediate step
         */
        Horizon clone();

        /** 
         * Return the time values  (Numpy requested)
         * @see getAttribute()
         */
        numpy::array getTime(
                vmMultiPoly2D region = 0);
        /** 
         * Return the depth values (Numpy requested)
         * @see getAttribute()
         */
        numpy::array getDepth(
                MultiPolygon2D = 0);
        /** 
         * Return an array 2d of the named attribute (Numpy requested)
         * @param poly region used to extract the values (default=0 :full horizon)
         */
        numpy::array getAttribute(string attr_name,
                MultiPolygon2D = 0);

        /** 
         * Set the time values  (Numpy requested)
         * @see setAttribute()
         */
        bool setTime(
                numpy::array npdata,
                MultiPolygon2D = 0);

        /** 
         * Return the depth values (Numpy requested)
         * @see setAttribute()
         */
        bool setDepth(
                numpy::array npdata,
                MultiPolygon2D = 0);
        /**
         * Set the values of the attribute (Numpy requested)
         * @param poly region used to extract the values (default=0 :full horizon)
         */
        bool setAttribute(
                string attr_name,
                numpy::array npdata,
                MultiPolygon2D = 0);

    };
};

#endif /* _Horizon_HPP_ */
