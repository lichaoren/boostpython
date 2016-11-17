


class VolumeMath():
    """This class supports tornado Volume Math (Util->Volume Math)

    .. note:: This feature works with interactive and G2 torpy module. Special interfaces for torpy module 
       are used: :func:`setFormula` , :func:`setInputVolumes` , :func:`setOutputPath` , :func:`calculate` . Although other APIs can
       still be used on Cluster job, the memory usage is not protected for large volumes

    Creating a VolumeMath object with setup parameters:

    >>> myMath=VolumeMath(hg,hrzs,True)
     
    :param hg: horizon group
    :type hg: :class:`HorizonGroup`
    :param hrzs: list of horizons
    :type hrzs: list[:class:`Horizon`]
    :param flag: True to fill up false to fill down
    :type flag: bool
    :param val: optional, out of boundary default value, if not set, default 0
    :type val: float
        
    :returns: No return value

    Creating a default VolumeMath object. The optional parameter can be used to set default out of boundary value:

    >>> myMath = VolumeMath(-9999.25)

    :param val: optional, out of boundary default value
    :type val: float 
    :returns: No return value 
    
    The VolumeMath class supports volume-volume and volume-float operations. 
    In :class:`Volume`, VolumeMath kernel is used to support some basic operators. 
    Combinations will make writing equation easier.

    For the explanation of the model parameters, please refer to the following figure:

    .. figure::  image/VolumeMath.png
            :alt: Matching Volume Math dialog parameters with scripting API

    - A few basic operations have been defined: 
    
         +------------------------------+----------------------------------------------------------+
         | opearations with one operand:| :func:`sin`, :func:`cos`, :func:`tan`,                   |
         |                              | :func:`arcsin`, :func:`arccos`, :func:`arctan`,          |
         |                              +----------------------------------------------------------+
         |                              | :func:`sqrt`, :func:`log10`, :func:`abs`,                |
         |                              +----------------------------------------------------------+
         |                              | :func:`d_dx`, :func:`d_dy`, :func:`d_dz`,                |
         |                              +----------------------------------------------------------+
         |                              | :func:`sum_top_down`, :func:`sum_btm_up`                 |
         +------------------------------+----------------------------------------------------------+
         | operations with two operands:| :func:`plus`, :func:`minus`,                             |
         |                              | :func:`multiply`, :func:`divide`,                        |
         |                              +----------------------------------------------------------+
         |                              | :func:`time_sin`, :func:`time_cos`, :func:`time_tan`,    |
         |                              +----------------------------------------------------------+
         |                              | :func:`time_arcsin`, :func:`time_arccos`,                | 
         |                              | :func:`time_arctan`,                                     |
         |                              +----------------------------------------------------------+
         |                              | :func:`min`, :func:`max`, :func:`mod`,                   |
         |                              | :func:`pow`, :func:`time_log10`                          |
         +------------------------------+----------------------------------------------------------+
    
    The VolumeMath class will try to respect the time_xxx operators that accept two operands 
    (same as used in Tornado dialog). 
    Also, NumPy style allows more natural expressions like **vol1*VolumeMath.sin(Vol2)** instead of **VolumeMath.time_sin(vol1,vol2)**
    
    Operations with one operand only accept :class:`Volume` as input. Operations with two operands must have at least one :class:`Volume`.
 
    Below is an example of doing Volume Math operations, without/with horizon as a constraint

    .. code-block:: python

       from algorithm import VolumeMath
       from base import *
       
       path="/data2/devtest/tornado/yanhliu/train_data/"
       data=Volume()
       data.load(path+'test_base.fdm.gz')
       
       #test the basic operators + - * / %
       data2=(100+data-300)*(data/20)%data
       save_path="/data2/devtest/tornado/yanhliu/Script_test/"
       data2.saveAs(save_path+'testPlus1.fdm')
       data3=VolumeMath.min(data,data2)

       #test the VolumeMath module with constraint
       #load hg
       hg=HorizonGroup()
       hg.load(path+'test_hg.hrz')
       hrzs=[]
       hrzs.append(hg.getHorizonByIndex(hg.getHorizonByName("TOS1")))
       
       myMath=VolumeMath(hg,hrzs,True)
       data3=myMath.d_dx(data)
       data3.saveAs(save_path+'testPlus2.fdm')

    Below is an example of doing Volume Math with g2 module script

    .. code-block:: python

       import base
       from algorithm import VolumeMath
       
       path='REMOTEFILE!houdat002:/data2/devtest/tornado/test_scripting/'
       calculator=VolumeMath()
       
       #naming convetion: VOL followed by number
       VOL0 = path+'xp_M_a_regrid.fdm'
       VOL1= path+ 'xp_M_a2_regrid.fdm'
       VOL2= path+ 'mod100.fdm'
       
       #put the volume names into the list according to the order
       calculator.setInputVolumes([VOL0, VOL1,VOL2])
       
       #set the output file name
       calculator.setOutputPath(path+'delete.fdm')
       
       #set the formula
       calculator.SetFormula('VOL0+VOL1*VOL2/10.')
       
       #do the calculation
       calculator.calculate()
 
    Below are the descriptions for each method:

    """

    def __init__(self, hg, hrzs, flag, val=0):
        pass

    def plus(self, arg1,arg2):
        """Returns arg1+arg2. If both arguments are volumes, addition is performed on values in corresponding locations. If one argument is float, it is added to each data value of the volume. 

        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> myMath = VolumeMath()
        >>> vol2 = myMath.plus(vol,5) #equivalent to vol2 = vol + 5

        :param arg1: left operand
        :type arg1: float or :class:`Volume`
        :param arg2: right operand
        :type arg2: float or :class:`Volume`
        :returns: new Volume
        :rtype: :class:`Volume`

        .. note:: At least one operand must be a Volume, otherwise the script will not run

        """
        pass
   
    def minus(self, arg1,arg2):
        """Returns arg1-arg2. If the arguments are volumes, the values of the second volume are subtracted from the corresponding locations in the first volume. If float, it is subtracted from all data values of the first argument.
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> myMath = VolumeMath()
        >>> vol2 = myMath.minus(vol,5) #equivalent to vol2 = vol - 5

        :param arg1: left operand
        :type arg1: float or :class:`Volume`
        :param arg2: right operand
        :type arg2: float or :class:`Volume`
        :returns: new Volume
        :rtype: :class:`Volume`
      
        .. note:: At least one operand must be a Volume, otherwise the script will not run

        .. note:: an expression **myMath.minus(5,vol)** is valid and the result is equivalent to **5 + (-1*vol)**

        """
        pass   

    def multiply(self, arg1,arg2):
        """Returns arg1*arg2. Multiply volumes' values at the corresponding locations, or multiply all data values of a volume by a float number.
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> vol2 = Volume()
        >>> vol2.load('path2') 
        >>> myMath = VolumeMath()
        >>> vol3 = myMath.multiply(vol,vol2) #equivalent to vol3 = vol2 * vol3

        :param arg1: left operand
        :type arg1: float or :class:`Volume` 
        :param arg2: right operand
        :type arg2: float or :class:`Volume` 
        :returns: new Volume
        :rtype: :class:`Volume`

        .. note:: At least one operand must be a Volume, otherwise the script will not run

        """
        pass

    def divide(self, arg1,arg2):
        """Returns arg1/arg2. Divide the volumes' values at the corresponding locations, or divide all data values of a volume by a float number.
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> vol2 = Volume()
        >>> vol2.load('path2') 
        >>> myMath = VolumeMath()
        >>> vol3 = myMath.divide(vol,vol2) #equivalent to vol3 = vol / vol2

        :param arg1: left operand
        :type arg1: float or :class:`Volume`  
        :param arg2: right operand
        :type arg2: float or :class:`Volume`  
        :returns: new Volume
        :rtype: :class:`Volume` 

        .. note:: At least one operand must be a Volume, otherwise the script will not run

        .. note:: an expression **myMath.divide(1,vol)** is valid and the result is equivalent to finding a reciprocal (1/V) for each data value (V) in the volume

        """
        pass


    def time_sin(self, arg1,arg2):
        """Returns arg1*sin(arg2). Multiply the first argument by a *sine* of the second argument. The second argument is assumed to be in radians
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> vol2 = Volume()
        >>> vol2.load('path2') 
        >>> myMath = VolumeMath()
        >>> vol3 = myMath.time_sin(vol,vol2) #equivalent to vol3 = vol * myMath.sin( vol2 )

        :param arg1: left operand
        :type arg1: float or :class:`Volume` 
        :param arg2: right operand
        :type arg2: float or :class:`Volume` 
        :returns: new Volume
        :rtype: :class:`Volume`

        .. note:: At least one operand must be a Volume, otherwise the script will not run

        """
        pass

    def sin(self, arg1):
        """Calculate *sine* of each data value in the volume. The volume data is assumed to be in radians.
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> myMath = VolumeMath()
        >>> vol2 = myMath.sin(vol) 

        :param arg1: volume to apply *sine* operation to
        :type arg1: :class:`Volume`  
        :returns: new Volume
        :rtype:  :class:`Volume` 

        """
        pass

    def time_cos(self, arg1,arg2):
        """Returns arg1*cos(arg2). Multiply the first argument by *cosine* of the second argument. The second argument is assumed to be in radians.
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> vol2 = Volume()
        >>> vol2.load('path2') 
        >>> myMath = VolumeMath()
        >>> vol3 = myMath.time_cos(vol,vol2) #equivalent to vol3 = vol * myMath.cos( vol2 )

        :param arg1: left operand
        :type arg1: float or :class:`Volume` 
        :param arg2: right operand
        :type arg2: float or :class:`Volume` 
        :returns: new Volume
        :rtype: :class:`Volume`

        .. note:: At least one operand must be a Volume, otherwise the script will not run

        """
        pass

    def cos(self, arg1):
        """Calculate the *cosine* of  each data value in the volume. The volume data is assumed to be in radians.
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> myMath = VolumeMath()
        >>> vol2 = myMath.cos(vol) 
               
        :param arg1: volume to apply *cosine* operation to
        :type arg1: :class:`Volume` 
        :returns: new Volume
        :rtype: :class:`Volume`

        """
        pass

    def time_tan(self, arg1,arg2):
        """Returns arg1*tan(arg2). Multiply the first argument by *tangent* of the second argument. The second argument is assumed to be in radians.
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> vol2 = Volume()
        >>> vol2.load('path2') 
        >>> myMath = VolumeMath()
        >>> vol3 = myMath.time_tan(vol,vol2) #equivalent to vol3 = vol * myMath.tan( vol2 )

        :param arg1: left operand
        :type arg1: float or :class:`Volume` 
        :param arg2: right operand
        :type arg2: float or :class:`Volume` 
        :returns: new Volume
        :rtype: :class:`Volume`

        .. note:: At least one operand must be a Volume, otherwise the script will not run

        """
        pass

    def tan(self, arg1):
        """Calculate the *tangent* of each data value in the volume. The volume data is assumed to be in radians.
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> myMath = VolumeMath()
        >>> vol2 = myMath.tan(vol) 
               
        :param arg1: volume to calculate *tangent* on
        :type arg1: :class:`Volume` 
        :returns: new Volume
        :rtype: :class:`Volume`

        """
        pass

    def time_arcsin(self, arg1,arg2):
        """Returns arg1*arcsin(arg2). Multiply the first argument by *arcsine* of the second argument. 
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> vol2 = Volume()
        >>> vol2.load('path2') 
        >>> myMath = VolumeMath()
        >>> vol3 = myMath.time_arcsin(vol,vol2) #equivalent to vol3 = vol * myMath.arcsin( vol2 )

        :param arg1: left operand
        :type arg1: float or :class:`Volume`  
        :param arg2: right operand
        :type arg2: float or :class:`Volume`  
        :returns: new Volume
        :rtype: :class:`Volume` 

        .. note:: At least one operand must be a Volume, otherwise the script will not run

        """
        pass

    def arcsin(self, arg1):
        """Calculate the *arcsine* of each data value in the volume.
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> myMath = VolumeMath()
        >>> vol2 = myMath.arcsin(vol) 
               
        :param arg1: volume to calculete *arcsine* on
        :type arg1:  :class:`Volume`  
        :returns: new Volume
        :rtype: :class:`Volume` 

        """
        pass

    def time_arccos(self, arg1,arg2):
        """Returns arg1*arccos(arg2). Multiply the first argument by *arccosine* of the second argument.
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> vol2 = Volume()
        >>> vol2.load('path2') 
        >>> myMath = VolumeMath()
        >>> vol3 = myMath.time_arccos(vol,vol2) #equivalent to vol3 = vol * myMath.arccos( vol2 )

        :param arg1: left operand
        :type arg1: float or :class:`Volume`  
        :param arg2: right operand
        :type arg2: float or :class:`Volume`  
        :returns: new Volume
        :rtype: :class:`Volume` 

        .. note:: At least one operand must be a Volume, otherwise the script will not run

        """
        pass

    def arccos(self, arg1):
        """Calculate the *arccosine* of each data value in the volume.
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> myMath = VolumeMath()
        >>> vol2 = myMath.arccos(vol) 
               
        :param arg1: volume to calculate *arccosine* on
        :type arg1: :class:`Volume`  
        :returns: new Volume
        :rtype: :class:`Volume` 

        """
        pass
    
    def time_arctan(self, arg1,arg2):
        """Returns arg1*arctan(arg2).  Multiply the first argument by *arctangent* of the second argument.
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> vol2 = Volume()
        >>> vol2.load('path2') 
        >>> myMath = VolumeMath()
        >>> vol3 = myMath.time_arctan(vol,vol2) #equivalent to vol3 = vol * myMath.arctan( vol2 )
               
        :param arg1: left operand
        :type arg1: float or :class:`Volume` 
        :param arg2: right operand
        :type arg2: float or :class:`Volume` 
        :returns: new Volume
        :rtype: :class:`Volume`

        .. note:: At least one operand must be a Volume, otherwise the script will not run

        """
        pass

    def arctan(self, arg1):
        """Calculate the *arctangent* of each data value in the volume.
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> myMath = VolumeMath()
        >>> vol2 = myMath.arctan(vol) 
               
        :param arg1: volume to calculate *arctangent* on
        :type arg1: :class:`Volume` 
        :returns: new Volume
        :rtype: :class:`Volume`

        """
        pass

    def time_log10(self, arg1,arg2):
        """Returns arg1*log10(arg2). Multiply the first argument by *common logarithm* of the second argument.
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> vol2 = Volume()
        >>> vol2.load('path2') 
        >>> myMath = VolumeMath()
        >>> vol3 = myMath.time_log10(vol,vol2) #equivalent to vol3 = vol * myMath.log10( vol2 )
               
        :param arg1: left operand
        :type arg1: float or :class:`Volume` 
        :param arg2: right operand
        :type arg2: float or :class:`Volume` 
        :returns: new Volume
        :rtype: :class:`Volume`

        .. note:: At least one operand must be a Volume, otherwise the script will not run

        """
        pass

    def log10(self, arg1):
        """Calculate *common logarithm* of each data value in the volume
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> myMath = VolumeMath()
        >>> vol2 = myMath.log10(vol) 
               
        :param arg1: volume to calculate *log10* on
        :type arg1: :class:`Volume` 
        :returns: new Volume
        :rtype: :class:`Volume`

        """
        pass

    def min(self, arg1,arg2):
        """Calculates the minimum of *arg1* and *arg2* for each location and output it into the new volume.
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> myMath = VolumeMath()
        >>> vol2 = myMath.min( myMath.max(vol,0), 1000 )  #vol2 values are within [0, 1000]
               
        :param arg1: left operand
        :type arg1: float or :class:`Volume` 
        :param arg2: right operand
        :type arg2: float or :class:`Volume` 
        :returns: new Volume
        :rtype: :class:`Volume`

        .. note:: At least one operand must be a Volume, otherwise the script will not run

        .. seealso:: :func:`max`

        """
        pass

    def max(self, arg1,arg2):
        """Calculates the maximum of *arg1* and *arg2* for each location and output it into the new volume
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> myMath = VolumeMath()
        >>> vol2 = myMath.max(vol,0)  #vol2 doesn't have any negative values
               
        :param arg1: left operand
        :type arg1: float or :class:`Volume` 
        :param arg2: right operand
        :type arg2: float or :class:`Volume` 
        :returns: new Volume
        :rtype: :class:`Volume`

        .. note:: At least one operand must be a Volume, otherwise the script will not run

        .. seealso:: :func:`min`

        """
        pass

    def sqrt(self, arg1):
        """Calculate square root of the *arg1*
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> myMath = VolumeMath()
        >>> vol2 = myMath.sqrt(vol)  
               
        :param arg1: volume to calculate square root on
        :type arg1: :class:`Volume` 
        :returns: new Volume
        :rtype: :class:`Volume`

        .. warning:: If the input volume has negative values, the output volume will contain NaN-s (not-a-number)

        """
        pass

    def mod(self, arg1,arg2):
        """Returns modulo division of first argument by second argument.
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> myMath = VolumeMath()
        >>> vol2 = myMath.mod(vol, 10.5)
               
        :param arg1: left operand
        :type arg1: float or :class:`Volume` 
        :param arg2: right operand
        :type arg2: float or :class:`Volume` 
        :returns: new Volume
        :rtype: :class:`Volume`

        .. note:: At least one operand must be a Volume, otherwise the script will not run

        """
        pass

    def pow(self, arg1,arg2):
        """Returns *arg1* to the power of *arg2*. 
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> myMath = VolumeMath()
        >>> vol2 = myMath.mod(vol, 2) # all values in vol2 are squared values from vol
               
        :param arg1: left operand
        :type arg1: float or :class:`Volume`  
        :param arg2: right operand
        :type arg2: float or :class:`Volume`  
        :returns: new Volume
        :rtype: :class:`Volume` 

        .. note:: At least one operand must be a Volume, otherwise the script will not run

        """
        pass

    def abs(self, arg1):
        """Returns absolute values from *arg1*
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> myMath = VolumeMath()
        >>> vol2 = myMath.abs(vol) #vol2 has no negative values

        :param arg1: volume to apply *abs* operation
        :type arg1: :class:`Volume` 
        :returns: new Volume
        :rtype: :class:`Volume`

        """
        pass

    def d_dx(self, arg1):
        """Returns derivative in x of (arg1)
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> myMath = VolumeMath()
        >>> vol2 = myMath.d_dx(vol)

        :param arg1: volume to calculate derivative on
        :type arg1: :class:`Volume` 
        :returns: new Volume
        :rtype: :class:`Volume`

        """
        pass

    def d_dy(self, arg1):
        """Returns derivative in y of (arg1)
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> myMath = VolumeMath()
        >>> vol2 = myMath.d_dy(vol)

        :param arg1: volume to calculate derivative on
        :type arg1: :class:`Volume` 
        :returns: new Volume
        :rtype: :class:`Volume`

        """
        pass

    def d_dz(self, arg1):
        """Returns derivative in z of (arg1)
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> myMath = VolumeMath()
        >>> vol2 = myMath.d_dz(vol)

        :param arg1: volume to calculate derivative on
        :type arg1: :class:`Volume` 
        :returns: new Volume
        :rtype: :class:`Volume`

        """
        pass

    def sum_top_down(self, arg1):
        """Returns summation of volume from the top down
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> myMath = VolumeMath()
        >>> vol2 = myMath.sum_top_down(vol)

        :param arg1: volume to calculate summation
        :type arg1: :class:`Volume` 
        :returns: new Volume
        :rtype: :class:`Volume`

        """
        pass

    def sum_btm_up(self, arg1):
        """Returns summation of volume from the bottom up
               
        Example:

        >>> from base import *
        >>> from algorithm import *
        >>> vol = Volume()
        >>> vol.load('path') 
        >>> myMath = VolumeMath()
        >>> vol2 = myMath.sum_btm_up(vol)

        :param arg1: volume to calculate summation
        :type arg1: :class:`Volume` 
        :returns: new Volume
        :rtype: :class:`Volume`

        """
        pass

    def setInputVolumes(self, lst):
        """ setup the input data paths

        .. note:: this function call is reserved for g2 torpy script

        :param lst: list of paths, eg: ['path0', 'path1', 'path2']
        :type lst: list 
        :returns: no return
        
        try to use :
        VOL0 = 'path0'
        VOL1 = 'path1'
        VOL2 = 'path2'
        lst = [VOL0, VOL1, VOL2] 
        to keep yourself clear about the data

        .. note:: For G2 job, add 'REMOTEFILE!houdat0XX:' before your path for Cluster accessing data island.
        .. warning:: only supports FDM (uncompressed) files

        """
        pass

    def setOutputPath(self, path):
        """ setup the path to be saved as
        .. note:: this function call is reserved for g2 torpy script

        :param path: path to be saved as
        :type path:  
        :returns: no return

        .. note:: For G2 job, add 'REMOTEFILE!houdat0XX:' before your path for Cluster accessing data island.
        .. warning:: only supports FDM (uncompressed) files

        """
        pass

    def setFormula(self, formula):
        """ setup the formula to calculate

        .. note:: this function call is reserved for g2 torpy script

        :param formula: the formula variables must be 'VOL0', 'VOL1' and so on. 
                        The index is identical to the index of list you set from :func:`setInputVolumes` .
        :type formula: string 
        :returns: no return

        the number of variables from formula must be equal to the number Volumes you set as input

        """
        pass

    def calculate(self):
        """ perform g2 torpy calculation

        .. note:: this function call is reserved for g2 torpy script
        :returns: no return

        """
        pass



if __name__ == "__main__":
    import doctest
    doctest.testmod()
    

