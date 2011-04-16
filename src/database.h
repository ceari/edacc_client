#ifndef __database_h__
#define __database_h__

#include <mysql/mysql.h>
#include <mysql/my_global.h>
#include <string>
#include <vector>
#include <map>
#include "datastructures.h"

using std::string;
using std::vector;
using std::map;

static const int DOWNLOAD_TIMEOUT = 30;
static const int DOWNLOAD_REFRESH = 25;

extern int database_connect(const string& hostname, const string& database,
							const string& username, const string& password,
							unsigned int port);
extern int database_query_select(string query, MYSQL_RES*& res);
extern int database_query_update(string query);
extern void database_close();


const char QUERY_INSERT_CLIENT[] = 
    "INSERT INTO Client (numCPUs, numCores, hyperthreading, turboboost,"
                         "CPUName, cacheSize, cpuflags, memory, memoryFree,"
                         "cpuinfo, meminfo, message, wait, reAdaptInterval)"
    "VALUES (%i, %i, %i, %i, '%s', %i, '%s', %llu, %llu, '%s', '%s', '%s', %i, %i);";
extern int insert_client();


const char QUERY_DELETE_CLIENT[] = "DELETE FROM Client WHERE idClient=%i;";
extern int delete_client(int client_id);


const char QUERY_POSSIBLE_EXPERIMENTS[] = 
    "SELECT Experiment.idExperiment, Experiment.name, Experiment.priority "
    "FROM ExperimentResults, Experiment_has_gridQueue JOIN gridQueue ON "
    "gridQueue_idgridQueue=idgridQueue JOIN Experiment ON idExperiment=Experiment_idExperiment "
    "WHERE ExperimentResults.Experiment_idExperiment = idExperiment AND "
    "idgridQueue=%d AND Experiment.active=TRUE AND status=-1 "
    "GROUP BY idExperiment HAVING COUNT(idJob) > 0;";
extern int get_possible_experiments(int grid_queue_id, vector<Experiment>& experiments);


const char QUERY_EXPERIMENT_CPU_COUNT[] = 
    "SELECT Experiment_idExperiment, SUM(Experiment_has_Client.numCores) "
    "FROM Experiment_has_Client "
    "GROUP BY Experiment_idExperiment;";
extern int get_experiment_cpu_count(map<int, int>& cpu_count_by_experiment);


const char QUERY_UPDATE_CORE_COUNT[] =
    "INSERT INTO Experiment_has_Client (Experiment_idExperiment, Client_idClient, "
                                        "numCores) "
    "VALUES (%i, %i, 1) ON DUPLICATE KEY UPDATE numCores=numCores+1";
extern int increment_core_count(int client_id, int experiment_id);

const char QUERY_DECREMENT_CORE_COUNT[] =
    "UPDATE Experiment_has_Client SET numCores=numCores-1 "
    "WHERE Experiment_idExperiment=%i AND Client_idClient=%i;";
extern int decrement_core_count(int client_id, int experiment_id);

const char LIMIT_QUERY[] = 
    "SELECT FLOOR(RAND()*COUNT(idJob)) as x FROM ExperimentResults \
    WHERE Experiment_idExperiment=%d AND status=-1 AND priority >= 0;";
const char SELECT_ID_QUERY[] = 
    "SELECT idJob FROM ExperimentResults WHERE Experiment_idExperiment=%d \
                             AND status=-1 AND priority >= 0 LIMIT %d,1;";
const char SELECT_FOR_UPDATE[] = 
    "SELECT idJob, SolverConfig_idSolverConfig, Experiment_idExperiment, "
    "Instances_idInstance, run, seed, priority, CPUTimeLimit, wallClockTimeLimit, "
    "memoryLimit, stackSizeLimit, outputSizeLimit "
    "FROM ExperimentResults WHERE idJob = %d and status=-1 FOR UPDATE;";
const char LOCK_JOB[] = 
    "UPDATE ExperimentResults SET status=0, startTime=NOW(), "
    "computeQueue=%d, computeNode='%s', computeNodeIP='%s' "
    "WHERE idJob=%d;";
extern int db_fetch_job(int grid_queue_id, int experiment_id, Job& job);

const char QUERY_GRID_QUEUE_INFO[] =
    "SELECT name, location, numNodes, numCPUs, walltime, "
    "availNodes, maxJobsQueue, description "
    "FROM gridQueue WHERE idgridQueue=%i;";
extern int get_grid_queue_info(int grid_queue_id, GridQueue& grid_queue);

const char QUERY_SOLVER[] =
	"SELECT Solver.idSolver, Solver.name, Solver.binaryName, Solver.md5 "
	"FROM Solver LEFT JOIN SolverConfig ON (idSolver = Solver_idSolver) "
	"WHERE idSolverConfig=%d;";

const char QUERY_INSTANCE[] =
	"SELECT name, md5 "
	"FROM Instances "
	"WHERE idInstance = %d;";
int get_solver(Job& job, Solver& solver);
int get_instance(Job& job, Instance& instance);

const char QUERY_SOLVER_BINARY[] =
	"SELECT `binary` "
	"FROM Solver "
	"WHERE idSolver = %d";

const char QUERY_INSTANCE_BINARY[] =
	"SELECT instance "
	"FROM Instances "
	"WHERE idInstance = %d";

const char QUERY_LOCK_INSTANCE[] =
	"INSERT INTO InstanceDownloads (idInstance, filesystemID, lastReport) "
	"VALUES (%d, %d, NOW());";

const char QUERY_LOCK_SOLVER[] =
	"INSERT INTO SolverDownloads (idSolver, filesystemID, lastReport) "
	"VALUES (%d, %d, NOW());";

const char QUERY_UPDATE_INSTANCE_LOCK[] =
	"UPDATE InstanceDownloads "
	"SET lastReport = NOW() "
	"WHERE idInstance = %d AND filesystemID = %d;";

const char QUERY_UPDATE_SOLVER_LOCK[] =
	"UPDATE SolverDownloads "
	"SET lastReport = NOW() "
	"WHERE idSolver = %d AND filesystemID = %d;";

const char QUERY_CHECK_INSTANCE_LOCK[] =
	"SELECT TIMESTAMPDIFF(SECOND, lastReport, NOW()) "
	"FROM InstanceDownloads "
	"WHERE idInstance = %d AND filesystemID = %d FOR UPDATE;";

const char QUERY_CHECK_SOLVER_LOCK[] =
	"SELECT TIMESTAMPDIFF(SECOND, lastReport, NOW()) "
	"FROM SolverDownloads "
	"WHERE idSolver = %d AND filesystemID = %d FOR UPDATE;";

const char QUERY_UNLOCK_INSTANCE[] =
	"DELETE FROM InstanceDownloads "
	"WHERE idInstance = %d AND filesystemID = %d;";

const char QUERY_UNLOCK_SOLVER[] =
	"DELETE FROM SolverDownloads "
	"WHERE idSolver = %d AND filesystemID = %d;";

class Solver_lock_update {
public:
	Solver* solver;
	int fsid;
	int finished;
};

class Instance_lock_update {
public:
	Instance* instance;
	int fsid;
	int finished;
};

int get_instance_binary(Instance& instance, string& instance_binary, int fsid);
int get_solver_binary(Solver& solver, string& solver_binary, int fsid);

const char QUERY_SOLVER_CONFIG_PARAMS[] =
    "SELECT idParameter, name, prefix, hasValue, defaultValue, `order`, value "
    "FROM Parameters JOIN SolverConfig_has_Parameters ON idParameter = Parameters_idParameter "
    "WHERE SolverConfig_idSolverConfig=%i ORDER BY `order`;";

extern int get_solver_config_params(int solver_config_id, vector<Parameter>& params);

const char QUERY_UPDATE_JOB[] = 
    "UPDATE ExperimentResults, ExperimentResultsOutput SET "
    "status=%d, resultCode=%d, resultTime=%f, solverOutput='%s', "
    "watcherOutput='%s', launcherOutput='%s', verifierOutput='%s', "
    "solverExitCode=%d, watcherExitCode=%d, verifierExitCode=%d "
    "WHERE idJob=%d AND ExperimentResults_idJob=%d;";
extern int db_update_job(const Job& job);

const char LOCK_MESSAGE[] =
    "SELECT message FROM Client WHERE idClient = %d FOR UPDATE;";
const char CLEAR_MESSAGE[] =
    "UPDATE Client SET message = '' WHERE idClient = %d";
int get_message(int client_id, string& message);

#endif