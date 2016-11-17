
namespace base {

    /**
     * Well data model.
     */
    class Well {
    public:

        string getWellName();
        string getWellID();

        float getVerticalShift();
        int getDepthType();

        base::WellLog addLog(string name);
        base::WellLog getLog(string name);
    }
} // namespace base
