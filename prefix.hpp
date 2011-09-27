/*
 * =====================================================================================
 *
 *       Filename:  prefix.hpp
 *
 *    Description:  Class for prefix object
 *
 *        Version:  1.0
 *        Created:  27/09/11 19:19:33
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Julio Cesar Garcia Vizcaino (garviz), garviz@garviz.mx
 *        Company:  CAOS
 *
 * =====================================================================================
 */

class prefix
{
    public:
        void set (string prefix) 
        {
            this.prefix = prefix;
        }
        void set (int counter)
        {
            this.counter = counter;
        }
        void set (string prefix, int counter)
        {
            this.prefix = prefix;
            this.counter = counter;
        }
        const string getPrefix() const
        {
            return prefix;
        }
        const int getCounter() const
        {
            return counter;
        }
    private:
        string prefix;
        int counter;
}
