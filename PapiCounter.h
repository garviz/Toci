#pragma once

#include <papi.h>

#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <assert.h>
#include <math.h>

#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

//#include "derived.h"

enum derivedStat{
    derived_FLOPS,
    derived_L1_DMR,
    derived_L2_DMR,
    derived_L1_TMR,
    derived_L2_TMR,
    derived_BANDWIDTH_SS,
    derived_BANDWIDTH_DS
};

template <typename T>
T vecSum(std::vector<T> const &v){
    T sum = T();
    for(int i=0; i<(int)v.size(); i++)
        sum += v[i];
    return sum;
}

template <typename T>
T vecMean(std::vector<T> const &v){
    return vecSum(v)/(T)v.size();
}

template <typename TVec>
void writeVecMatlab(std::ofstream &fid, std::string name, TVec const &v){
    fid << name << " = [";
    for(int i=0; i<v.size(); i++)
        fid << v[i] << (i<(v.size()-1) ? " " : "];");
    fid << std::endl;
}

// enumerate the different output formats for counter information
// LaTex support not currently implemented
enum papiFileFormat {fileFormatMatlab, fileFormatPlain, fileFormatLaTeX};

// singleton that handles papi intialisation and access to papi calls
class Papi{
public:
    static Papi* instance();
    void init();
    int numEvents(){
        return eventNames_.size();
    };
    std::string const &eventName(int i){
      assert(i<numEvents());
      return eventNames_[i];
    };
    int eventNumber(int i){
      assert(i<numEvents());
      return events_[i];
    };
    long long counter(int tid, int i){
      assert(i<numEvents());
      assert(tid<numThreads());
      return counters_[tid][i];
    };
    double time(int tid){
      assert(tid<numThreads());
      return timeVec_[tid];
    };
    void startCounters();
    void stopCounters();
    int numThreads() {return numThreads_;};
    bool counting() { return counting_; };
private:
    Papi(Papi const &) {};
    Papi() {setup_=false; debug_=false; counting_=false;};

    void papi_print_error(int);

    bool setup_;
    bool debug_;
    bool counting_;
    int eventSet_;
    int numThreads_;
    std::vector<std::string> eventNames_;
    std::vector<int> events_;
    std::vector<double> timeVec_;
    std::vector<std::vector<long long> > counters_;

    static Papi* instance_;
};

class PapiCounter{
public:
    PapiCounter();
    void start();
    void stop();
    void writeToFile(  std::string const &name,
                       int eventID,
                       std::ofstream &fid,
                       papiFileFormat fileFormat);

    std::string name(int i)   { return names_[i]; };
    int         number(int i) { return numbers_[i]; };
    long long   count(int tid, int i)  { assert(tid<threads()); return counts_[tid][i]; };
    double      time(int tid)          { assert(tid<threads()); return times_[tid]; };
    double      time()          { return vecMean(times_); };
    int         counters()    { return names_.size(); };
    int         threads()     { return Papi::instance()->numThreads(); };
    long long   count(int i);
    void        printScreen();
    std::vector<long long> counts(int i);
private:
    std::vector<std::string> names_;
    std::vector<int>         numbers_;
    std::vector<double>      times_;
    std::vector<std::vector<long long> >   counts_;
    bool derivedStatAvailable(derivedStat statIDX);
    std::vector<double> derivedStatCompute(derivedStat statIDX);
};

// class to manage all events that we want to benchmark
//      essentially a wrapper around map<string, PapiCoutner>
class PapiCounterList{
public:
    PapiCounterList() {};
    void writeToFile(std::string fname, papiFileFormat=fileFormatPlain);
    void writeToFile(std::ofstream &fid, papiFileFormat=fileFormatPlain);
    void printScreen();
    void addEvent(std::string ename);
    PapiCounter& event(std::string ename);

    // override [] to allow access to events using ["eventName"]
    PapiCounter& operator[] (std::string &str) { return event(str); };
    PapiCounter& operator[] (std::string str) { return event(str); };
private:
    std::map<std::string, PapiCounter> events_;
};

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
//// REPLACEMENT FOR CPP SOURCE FILE
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

std::string derivedStatName(derivedStat statIDX){
    switch(statIDX){
        case derived_FLOPS:
            return std::string("derived_FLOPS");
            break;
        case derived_L1_DMR:
            return std::string("derived_L1_DMR");
            break;
        case derived_L2_DMR:
            return std::string("derived_L2_DMR");
            break;
        case derived_L1_TMR:
            return std::string("derived_L1_TMR");
            break;
        case derived_L2_TMR:
            return std::string("derived_L2_TMR");
            break;
        case derived_BANDWIDTH_SS:
            return std::string("derived_BANDWIDTH_SS");
            break;
        case derived_BANDWIDTH_DS:
            return std::string("derived_BANDWIDTH_DS");
            break;
    }
    return std::string("");
}

int findString(std::vector<std::string> const& strVec, std::string str){
    std::vector<std::string>::const_iterator it;
    it = std::find(strVec.begin(), strVec.end(), str);
    // return -1 if str not found in strVec
    if(it==strVec.end())
        return -1;
    return it - strVec.begin();
}

/*=================================================
                    PAPI
=================================================*/

// initialise Papi instance
Papi* Papi::instance_ = 0;

Papi* Papi::instance(){
    return instance_ ? instance_ : (instance_ = new Papi);
}

void Papi::init(){
    // only initialise if not already initialised
    if(setup_)
        return;

    int papi_error;

    // set debugging if requested by environment variable
    char *debugStr = getenv("PAPI_DEBUG");
    debug_ = (debugStr != NULL);
    if(debug_)
        std::cerr << "Papi debug mode on" << std::endl;

    // initialize the papi library */
    papi_error = PAPI_library_init(PAPI_VER_CURRENT);
    if (papi_error != PAPI_VER_CURRENT) {
        std::cerr << "PAPI library init error!" << std::endl;
        exit(1);
    }
#ifdef _OPENMP
    // assume fixed thread affinity, otherwise this approach fails
    papi_error = PAPI_thread_init((long unsigned int (*)()) omp_get_thread_num);
    if ( papi_error != PAPI_OK ){
        std::cerr << "Could not initialize the library with openmp."
                  << std::endl;
        exit(1);
    }
    numThreads_ = omp_get_max_threads();
#else
    numThreads_ = 1;
#endif

    timeVec_.resize(numThreads_);

    // determine the number of hardware counters
    int num_hwcntrs;
    papi_error = num_hwcntrs = PAPI_num_counters();
    if (papi_error <= PAPI_OK){
        std::cerr << "PAPI error : unable to determine number of hardware counters" << std::endl;
        papi_print_error(papi_error);
        exit(1);
    }
    if( debug_ )
        std::cout << "There are " << num_hwcntrs
                  << " hardware counters available" << std::endl;

    // get user-defined list of hardware counters from environment variable
    char *papi_counters = getenv("PAPI_EVENTS");
    if (debug_)
        std::cout << "PAPI_EVENTS = " <<  papi_counters << std::endl;

    char *result = NULL;
    char  delim[] = "|";

    result = strtok( papi_counters, delim );
    while( result != NULL ){
        int eventID;
        papi_error = PAPI_event_name_to_code(result, &eventID);
        if(     papi_error==PAPI_OK
            &&  std::find(events_.begin(), events_.end(),eventID)==events_.end() )
        {
            eventNames_.push_back(std::string(result));
            events_.push_back(eventID);
        }
        else{
            std::cerr << "Papi Error : not adding event : " << result << std::endl;
        }
        result = strtok( NULL, delim );
    }
    if (debug_){
        std::cout << "there are " << eventNames_.size()
                  << " requested counters" << std::endl;
    }

    if(numEvents()==0){
        setup_ = true;
        return;
    }

    if (numEvents() > 127){
        std::cerr << "Too many events selected : exiting" << std::endl;
        exit(-1);
    }

    eventSet_ = PAPI_NULL;
    papi_error = PAPI_create_eventset(&eventSet_);

    if(papi_error != PAPI_OK){
        std::cerr << "Papi error : Could not create the EventSet" << std::endl;
        papi_print_error(papi_error);
        exit(-1);
    }

    if(debug_)
        for(int i = 0; i < numEvents(); i++)
            std::cerr << "Event " << i << " out of " << numEvents()
                      << " = " << eventName(i) << std::endl;

    // allocate space for counters
    counters_.resize(numThreads_);
    for(int i=0; i<numThreads_; i++)
        counters_[i].resize(numEvents());

    setup_ = true;
}

void Papi::papi_print_error(int ierr){
    char errstring[PAPI_MAX_STR_LEN];
    PAPI_perror(errstring);

    std::cerr << "PAPI error " << errstring << std::endl;
}

void Papi::startCounters(){
    if(!setup_)
        init();
    if(counting()){
        std::cerr << "PAPI counters error : cannot start papi counters when they are already running"
                  << std::endl;
        exit(-1);
    }

    #pragma omp parallel
    {
        if(numEvents()){
            int papi_error = PAPI_start_counters(&events_[0], events_.size());
            if (papi_error != PAPI_OK){
                std::cerr << "PAPI error : unable to start counters" << std::endl;
                papi_print_error(papi_error);
                exit(-1);
            }
        }

        // TODO : replace the wtime() with non-openmp equivalent for !_OPENMP
        double ttmp = omp_get_wtime();
#ifdef _OPENMP
        int tid = omp_get_thread_num();
#else
        int tid = 1;
#endif
        timeVec_[tid] = -ttmp;
    }
    counting_ = true;
}


void Papi::stopCounters(){
    if(!counting()){
        std::cerr << "PAPI counters error : cannot stop papi counters when they are have not been started" << std::endl;
        exit(-1);
    }

    #pragma omp parallel
    {
#ifdef _OPENMP
        int tid = omp_get_thread_num();
#else
        int tid = 1;
#endif
        if(numEvents()){
            int papi_error = PAPI_stop_counters(&counters_[tid][0], events_.size());
            if (papi_error != PAPI_OK){
                std::cerr << "PAPI error : unable to stop counters" << std::endl;
                papi_print_error(papi_error);
                exit(-1);
            }
        }
        // TODO : replace the wtime() with non-openmp equivalent for !_OPENMP
        timeVec_[tid] += omp_get_wtime();
    }
    counting_ = false;
}

/*=================================================
                PapiCounter
=================================================*/
PapiCounter::PapiCounter(){
    Papi::instance()->init();

    int numCounters = Papi::instance()->numEvents();
    int numThreads = Papi::instance()->numThreads();
    for(int i=0; i<numCounters; i++){
        names_.push_back(Papi::instance()->eventName(i));
        numbers_.push_back(Papi::instance()->eventNumber(i));
    }
    counts_.resize(numThreads);
    for(int tid=0; tid<numThreads; tid++){
        counts_[tid].resize(numCounters,0LL);
    }
    times_.resize(numThreads);
}

void PapiCounter::stop(){
    Papi::instance()->stopCounters();
    int numCounters = Papi::instance()->numEvents();
    int numThreads = Papi::instance()->numThreads();
    for(int tid=0; tid<numThreads; tid++){
        for(int i=0; i<numCounters; i++){
            counts_[tid][i] += Papi::instance()->counter(tid,i);
        }
        times_[tid] += Papi::instance()->time(tid);
    }
}

void PapiCounter::start(){
    Papi::instance()->startCounters();
}

long long PapiCounter::count(int i){
    assert( i<counters() );
    long long sum = 0LL;
    for(int tid=0; tid<threads(); tid++)
        sum += count(tid,i);
    //return sum/(long long)threads();
    return sum;
}

std::vector<long long> PapiCounter::counts(int i){
    assert( i<counters() );
    std::vector<long long> tmp;
    for(int tid=0; tid<threads(); tid++)
        tmp.push_back(count(tid,i));
    return tmp;
}

void PapiCounter::writeToFile(std::string const &runName, int eventID, std::ofstream &fid, papiFileFormat fileFormat){
    if( counters() ){
        int numThreads = Papi::instance()->numThreads();
        switch( fileFormat ){
            case fileFormatPlain:
                fid << "----------------------------" << std::endl;
                fid << runName << " :: wall time " << time() << " s" <<  std::endl;
                fid << "----------------------------" << std::endl;
                if(Papi::instance()->numThreads()>1){
                    for(int tid=0; tid<Papi::instance()->numThreads(); tid++){
                        fid << "   THREAD" << std::setw(2) << tid;
                    }
                }
                fid << " [      TOTAL ]" << std::endl;
                for(int i=0; i<counters(); i++){
                    if(Papi::instance()->numThreads()>1){
                        for(int tid=0; tid<Papi::instance()->numThreads(); tid++){
                            fid << " " << std::setw(10) << count(tid, i);
                        }
                    }
                    fid << " [ " << std::setw(10) << count(i) << " ]"
                        << "\t" << name(i)  << std::endl;
                }
                /*
                for(int i in derivedCounters<platform>::counters){
                    if( derivedCounters<platform>::counters[i].is_available() )
                        compute;
                        write to screen;
                }
                */
                if( derivedStatAvailable(derived_BANDWIDTH_SS) ){
                    std::vector<double> stat = derivedStatCompute(derived_BANDWIDTH_SS);
                    if(numThreads>1)
                        for(int tid=0; tid<numThreads; tid++)
                            fid << "     -     ";
                    fid << " [ " << std::setw(10) << vecSum(stat)/vecMean(times_)/(1.e6) << " ]"
                        << "\tderived_BANDWIDTH_SS (MB/s)" << std::endl;
                }
                if( derivedStatAvailable(derived_BANDWIDTH_DS) ){
                    std::vector<double> stat = derivedStatCompute(derived_BANDWIDTH_DS);
                    if(numThreads>1)
                        for(int tid=0; tid<numThreads; tid++)
                            fid << "     -     ";
                    fid << " [ " << std::setw(10) << vecSum(stat)/vecMean(times_)/(1.e6) << " ]"
                        << "\tderived_BANDWIDTH_DS (MB/s)" << std::endl;
                }

                break;
            case fileFormatMatlab:
                fid << runName << " = " << eventID << ";" << std::endl;
                for(int i=0; i<counters(); i++){
                    std::stringstream vname;
                    vname << "event{"<<runName<<"}.counter(" << i+1 << ").count";
                    fid << "event{"<<runName<<"}.counter(" << i+1 << ").name = \'"
                        << name(i)    << "\';" << std::endl;
                    writeVecMatlab(fid, vname.str(), counts(i));
                }
                break;
            case fileFormatLaTeX:
                fid << "\\hline" << std::endl;
                fid << "\\multicolumn{2}{c}{" << runName << "}" << std::endl;
                fid << "\\hline" << std::endl;
                fid << "counter & count" << "\\\\" << std::endl;
                fid << "\\hline" << std::endl;
                for(int i=0; i<counters(); i++)
                    fid << "\\lst{" << name(i) << "}"
                        << " & " << count(i) << "\\\\" << std::endl;
                break;
        }
    }
}

void PapiCounter::printScreen(){
    int numThreads=Papi::instance()->numThreads();
    if(counters()>0){
        if(Papi::instance()->numThreads()>1){
            for(int tid=0; tid<Papi::instance()->numThreads(); tid++){
                std::cout << "   THREAD" << std::setw(2) << tid;
            }
        }
        std::cout << " [      TOTAL ]" << std::endl;
        for(int i=0; i<counters(); i++){
            if(Papi::instance()->numThreads()>1){
                for(int tid=0; tid<Papi::instance()->numThreads(); tid++){
                    std::cout << " " << std::setw(10) << count(tid, i);
                }
            }
            std::cout << " [ " << std::setw(10) << count(i) << " ]"
                      << "\t" << name(i)  << std::endl;
        }
        if( derivedStatAvailable(derived_BANDWIDTH_SS) ){
            std::vector<double> stat = derivedStatCompute(derived_BANDWIDTH_SS);
            if(numThreads>1)
                for(int tid=0; tid<numThreads; tid++)
                    std::cout << "     -     ";
            std::cout << " [ " << std::setw(10) << vecSum(stat)/vecMean(times_)/(1.e6) << " ]"
                << "\tderived_BANDWIDTH_SS (MB/s)" << std::endl;
        }
        if( derivedStatAvailable(derived_BANDWIDTH_DS) ){
            std::vector<double> stat = derivedStatCompute(derived_BANDWIDTH_DS);
            if(numThreads>1)
                for(int tid=0; tid<numThreads; tid++)
                    std::cout << "     -     ";
            std::cout << " [ " << std::setw(10) << vecSum(stat)/vecMean(times_)/(1.e6) << " ]"
                << "\tderived_BANDWIDTH_DS (MB/s)" << std::endl;
        }
    }else{
        std::cout << "PAPI-WRAP :: no counters to print" << std::endl;
    }
}

bool PapiCounter::derivedStatAvailable(derivedStat statIDX){
    std::vector<std::string>::iterator it1;
    switch(statIDX){
        case derived_FLOPS:
            return findString(names_, std::string("PAPI_FP_INS"))>=0 ? true : false;
        case derived_L1_DMR:
            return(
                findString(names_, std::string("PAPI_L1_DCA"))>=0 &&
                findString(names_, std::string("PAPI_L1_DCM"))>=0
            );
        case derived_L2_DMR:
            return(
                findString(names_, std::string("PAPI_L2_DCA"))>=0 &&
                findString(names_, std::string("PAPI_L2_DCM"))>=0
            );
        case derived_L1_TMR:
            return(
                findString(names_, std::string("PAPI_L1_TCA"))>=0 &&
                findString(names_, std::string("PAPI_L1_TCM"))>=0
            );
        case derived_L2_TMR:
            return(
                findString(names_, std::string("PAPI_L2_TCA"))>=0 &&
                findString(names_, std::string("PAPI_L2_TCM"))>=0
            );
        case derived_BANDWIDTH_SS:
            return(
                findString(names_, std::string("SYSTEM_READ_RESPONSES:0x07"))>=0 &&
                findString(names_, std::string("OCTWORD_WRITE_TRANSFERS:0x01"))>=0
            );
        case derived_BANDWIDTH_DS:
            return(
                findString(names_, std::string("SYSTEM_READ_RESPONSES:0x07"))>=0 &&
                findString(names_, std::string("OCTWORD_WRITE_TRANSFERS:0x01"))>=0
            );
    }
    return false;
}

std::vector<double> PapiCounter::derivedStatCompute(derivedStat statIDX){
    std::vector<double> derived(threads());
    int idx, idxCM, idxCA;
    int idxSRS, idxOWT;
    switch(statIDX){
        case derived_FLOPS:
            idx = findString(names_, std::string("PAPI_FP_INS"));
            for(int tid=0; tid<threads(); tid++)
                // returns total number of FP operations
                // determine FLOPS by summing and dividing by time
                derived[tid] = count(tid, idx);
            return derived;
            break;
        case derived_L1_DMR:
            idxCM = findString(names_, std::string("PAPI_L1_DCM"));
            idxCA = findString(names_, std::string("PAPI_L1_DCA"));
            for(int tid=0; tid<threads(); tid++)
                derived[tid] = (double)count(tid, idxCM)/(double)count(tid, idxCA);
            return derived;
        case derived_L2_DMR:
            idxCM = findString(names_, std::string("PAPI_L2_DCM"));
            idxCA = findString(names_, std::string("PAPI_L2_DCA"));
            for(int tid=0; tid<threads(); tid++)
                derived[tid] = (double)count(tid, idxCM)/(double)count(tid, idxCA);
            return derived;
        case derived_L1_TMR:
            idxCM = findString(names_, std::string("PAPI_L1_TCM"));
            idxCA = findString(names_, std::string("PAPI_L1_TCA"));
            for(int tid=0; tid<threads(); tid++)
                derived[tid] = (double)count(tid, idxCM)/(double)count(tid, idxCA);
            return derived;
        case derived_L2_TMR:
            idxCM = findString(names_, std::string("PAPI_L2_TCM"));
            idxCA = findString(names_, std::string("PAPI_L2_TCA"));
            for(int tid=0; tid<threads(); tid++)
                derived[tid] = (double)count(tid, idxCM)/(double)count(tid, idxCA);
            return derived;
        // see page 2101 of Shirley Moore et al., Procedia Computer Science 4 (2011)
        // this needs some tweaking for Interlagos (15h):
        case derived_BANDWIDTH_SS:
            idxSRS = findString(names_, std::string("SYSTEM_READ_RESPONSES:0x07"));
            idxOWT = findString(names_, std::string("OCTWORD_WRITE_TRANSFERS:0x01"));
            for(int tid=0; tid<threads(); tid++)
                derived[tid] = (double)count(tid, idxSRS)*32. + (double)count(tid, idxOWT)*8.;
            return derived;
        case derived_BANDWIDTH_DS:
            idxSRS = findString(names_, std::string("SYSTEM_READ_RESPONSES:0x07"));
            idxOWT = findString(names_, std::string("OCTWORD_WRITE_TRANSFERS:0x01"));
            // dual stream: accumulate bandwidth of one core per Bulldozer module
            for(int tid=0; tid<threads(); tid++)
                derived[tid] = (double)count(tid, idxSRS)*32. + (double)count(tid, idxOWT)*4.;
            return derived;
    }
    // this will never occur - just to keep the compiler warnings off
    return std::vector<double>(0);
}

/*================================================
            PapiCounterList
================================================*/
void PapiCounterList::addEvent(std::string ename){
    // ensure that someone hasn't already added an event with this name
    assert( events_.find(ename)==events_.end() );

    events_[ename] = PapiCounter();
}

PapiCounter& PapiCounterList::event(std::string ename){
    // ensure that an event with ename exists
    assert( events_.find(ename)!=events_.end() );

    return events_[ename];
}

void PapiCounterList::writeToFile(std::string fname, papiFileFormat fileFormat){
    std::ofstream fid;
    fid.open(fname.c_str());

    switch(fileFormat){
        case fileFormatMatlab:
            break;
        case fileFormatPlain:
            break;
        case fileFormatLaTeX:
            fid << "\\begin{tabular}{lr}" << std::endl;
            break;
    }

    int id=1;
    for( std::map<std::string, PapiCounter>::iterator it = events_.begin();
         it != events_.end();
         it++ ){
        it->second.writeToFile(it->first, id, fid, fileFormat);
        id++;
    }

    switch(fileFormat){
        case fileFormatMatlab:
            break;
        case fileFormatPlain:
            break;
        case fileFormatLaTeX:
            fid << "\\hline" << std::endl;
            fid << "\\end{tabular}" << std::endl;
            break;
    }

    fid.close();
}

void PapiCounterList::writeToFile(std::ofstream &fid, papiFileFormat fileFormat){
    switch(fileFormat){
        case fileFormatMatlab:
            break;
        case fileFormatPlain:
            break;
        case fileFormatLaTeX:
            fid << "\\begin{tabular}{lr}" << std::endl;
            break;
    }

    int id=1;
    for( std::map<std::string, PapiCounter>::iterator it = events_.begin();
         it != events_.end();
         it++ ){
        it->second.writeToFile(it->first, id++, fid, fileFormat);
    }

    switch(fileFormat){
        case fileFormatMatlab:
            break;
        case fileFormatPlain:
            break;
        case fileFormatLaTeX:
            fid << "\\hline" << std::endl;
            fid << "\\end{tabular}" << std::endl;
            break;
    }

    // close the file stream
    fid.close();
}

void PapiCounterList::printScreen(){
    for( std::map<std::string, PapiCounter>::iterator it = events_.begin();
         it != events_.end();
         it++ ){
        std::cout << "--------------------------------" << std::endl;
        std::cout << it->first << " :: wall time " << it->second.time() << " s" <<  std::endl;
        std::cout << "--------------------------------" << std::endl;
        it->second.printScreen();
    }
}

