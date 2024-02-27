#define fmin dummy_fmin
#include <math.h>
#undef fmin
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <float.h>
#include <string.h>
#include <assert.h>
#include "nr.h"
#include "nrutil.h"
#include "mex.h"
#include "matrix.h"
#include "mat.h"

// Misc globals
#define SEP_CHAR "//"
#define WRITE_SELFSHIELD 0

// Mathematical globals
//#define M_PI 3.14159265359
//#define M_LN10  2.302585093
//#define M_LN2 0.693147181

// SOLVE_FLUX globals
#define DREFL 0.84     // Diffusion coefficient for graphite reflector [cm]; it's only a weak function of energy, so we take it to be constant
#define SAREFL 0.00038 // macroscopic 0.0253 eV absorption xs for graphite reflector; we assume 1/v dependence
#define EPS 1.0e-7

// DOWNSCATTER globals
#define DELTAL M_LN10  // For deriving group width
#define NTH 40         // Number of thermal groups
#define NFAST 700      // Number of fast (LO+HI) groups
#define NTOT NTH+NFAST
#define DELTATH 1./10.    // Width of thermal group, units of ln_10
#define DELTAFAST 1./100. // Width of fast group, units of ln_10
#define TRUE 1
#define FALSE 0
#define FINEDS 97  // Downscattering spans no more than 97 fine groups (except for hydrogen-1)
#define COARSEDS 11  // Downscattering spans no more than 11 coarse groups (except for hydrogen-1)

// V:BUDS GLOBALS
#define N_COARSE 400
#define N_FINE 2000

// SELFSHIELD globals

#define BASIS pow(10.,24.)

// GET_PROBS globals
#define NUM_POINTS_LOOKUP 9   // number of points per r/p in "lookup"

// ECON_MODULE globals
int NCATS;  // We allow up to 3 fuel cycle 'categories': U, Pu, other actinides; this reflects the fact that many costs, for instance fuel fabrication, may depend on the type of material being fabricated as well as the mass flow
int NFLOWS;  // I think we need to specify the mass at 8 distinct points in the cycle

// ***** To change the above constants, alter the numbers in the array dimensions *below*

// LOAD globals

#define NTH 40
#define NLOHI 700
#define NLO 200
#define NHI 500
#define NTEMPS 5
#define NBOUNDTEMPS 3
#define ACTINIDE_PATH "outputfiles"
#define MESH_PATH "../data/meshes"
#define MAX_E 10000000.
#define NACT 24
#define FP_PROXY_NUC "ag-109"
#define FP_PROXY_TEMP 900.
#define FP_BASENAME "lumped_fp"
#define FLUX_BASENAME "./flux/fluxes"

#define N_AVO 0.6022

#define USE28INELSCATKERNEL

/* define indices */
#define I_U_232 0
#define I_U_233 1
#define I_U_234 2
#define I_U_235 3
#define I_U_236 4
#define I_U_238 5
#define I_PU_236 6
#define I_PU_238 7
#define I_PU_239 8
#define I_PU_240 9
#define I_PU_241 10
#define I_PU_242 11
#define I_TH_230 12
#define I_TH_232 13
#define I_PA_231 14
#define I_NP_237 15
#define I_AM_241 16
#define I_AM_242 17
#define I_AM_243 18
#define I_CM_242 19
#define I_CM_243 20
#define I_CM_244 21
#define I_CM_245 22
#define I_CM_246 23


/* these five temperatures are the temperatures for which data was
 * extracted from the endf files.  for all other temperatures,
 * interpolated values will be used. */
float temps[NTEMPS] = {300,600,900,1200,1500};

/* this global boolean should be set to one if we are loading a
 * bound nuclide, so that we interpolate using the appropriate
 * temperature list.  make sure this is set back to zero afterwards.
 * only interpolate_nonactinides checks this variable */
/*int is_bound=0;
*/
/* use these temperatures for looking up bound nuclides */
float bound_temps[NBOUNDTEMPS] = {296,600,1000};

/* these are the group endpoints for the thermal and fast energy
 * lattices.  they need to be loaded from file at runtime using
 * load_meshes */
float meshth[NTH+1];
float meshlh[NLOHI+1];

/* object actinide: a structure containing all the relevant values
 * for an actinide.  the stuct contains two substructs, one for
 * thermal energies and the other for fast energies.  both substructs
 * contain arrays for scattering, absorption and fission cross-
 * sections, nubar, and scattering cosines.  in addition, the thermal
 * substruct contains values for the scattering kernel, and the fast
 * substruct contains values for the self-shielding coefficients. */
typedef struct ther {
        float scat[NTH];
        float absp[NTH];
        float fiss[NTH];
        float nubr[NTH];
        float scos[NTH];
        float sker[NTH][NTH];
} ther;

typedef struct lohi {
        float scat[NLOHI];
        float absp[NLOHI];
        float fiss[NLOHI];
        float ffct[NLOHI][6];
        float nubr[NLOHI];
        float scos[NLOHI];
    float inxs[NHI];
        float inkl[NHI][NHI];
} lohi;

typedef struct actinide {
        ther th;
        lohi lh;
        int a;
        float lambda;
} actinide;

actinide actinide_array[NACT];
float actinide_fudge[NACT][2];

char *act_basenames[NACT] = {
        "u-232",
        "u-233",
        "u-234",
        "u-235",
        "u-236",
        "u-238",
        "pu-236",
        "pu-238",
        "pu-239",
        "pu-240",
        "pu-241",
        "pu-242",
        "th-230",
        "th-232",
        "pa-231",
        "np-237",
        "am-241",
        "am-242m",
        "am-243",
        "cm-242",
        "cm-243",
        "cm-244",
        "cm-245",
        "cm-246"
};

float act_amu[NACT] = { 232, 233, 234, 235, 236, 238, 236, 238, 239, 240, 241, 242,
                        230, 232, 231, 237, 241, 242, 243, 242, 243, 244, 245, 246};

float act_a[NACT] = {92, 92, 92, 92, 92, 92, 94, 94, 94, 94, 94, 94,
                        90, 90, 91, 93, 95, 95, 95, 96, 96, 96, 96, 96};

#ifdef USE28INELSCATKERNEL
float globalinel[NHI][NHI];
float globalinel_c[NHI/10][NHI/10];
#endif

/* object nonactinide: like actinide, but without the fission
 * cross section and without nubar */
typedef struct nther {
        float scat[NTH];
        float absp[NTH];
        float scos[NTH];
        float sker[NTH][NTH];
} nther;

typedef struct nlohi {
        float scat[NLOHI];
        float absp[NLOHI];
        float ffct[NLOHI][6];
        float scos[NLOHI];
} nlohi;

typedef struct nonactinide {
        nther th;
        nlohi lh;
        int a;
} nonactinide;

/* declare nonactinide proxy for the fission products */
nonactinide fp;

/* declare fission spectrum array */
float fiss_spec[NLOHI];
float fiss_spec_c[NLOHI/10];


typedef struct CostStruct
  {   // This structure contains data concerning a cost/revenue: type, lead/lag time with respect to fuel charge, cost payment duration, mass type associated with cost.
    float UnitCost;  // units are $/kg or $/kg/yr
    float Var;  // Variance of unit cost
    float PartialCost; // Used to store Cost/UnitCost for cost difference variance computation
    float LagTime;  // lag time from fuel dischcharge from reactor; negative number is lead time from fuel charge
    float Duration;  // duration of liability/revenue; defaults to zero for certain cost types
    float UnitVar[2]; // holds miscellaneous data: for CostType= 2 or 3, it holds rate of return of fund; for CostType= 4, holds escalation rate of cost, for CostType=5, both fund rate of return and cost escalation rate
    int CostType; // unit cost type: 0=one-time cost, 1=distributed cost, 2=one time cost, distributed payment (e.g. sinking fund), 3=distributed cost, distributed payment (e.g. annual HLW storage fee), 4=Escalating distributed cost, one time payment, 5=Escalating distributed cost, distributed payment
    int MassType[3];  // Mass flow type associated with cost: 0=not relevant to this category, 1=ore, 2=pre-enrich feed, 3=SWU, 4=fuel into reactor, 5=fuel reactor->DD,6=fuel reactor->rep, 7=post-reprocess HLW, 8=post-reprocess recovered material #1.  categories: 0=U cycle, 1=Pu cycle, 2=other actinide cycle  **NCATS
    char Name[30]; // Name of the cost;
    float CostSum; // the accumulated cost derived for this category
    struct CostStruct *NextCost;
  } COSTSTR;

typedef struct CycStruct
  {  // Contains all necessary information for one cycle
    float RefTime; // Time of fuel charge for a cycle with respect to overall reference of initial fuel charge in first reactor: relevant for reprocessing scenarios
    int Category; // Classification of cycle; see MassType for definitions
    float MassFlow[9]; // Relevant masses for each mass flow type are stored here; MassFlow[0] is fuel burnup [MWd/kg]*thermal efficiency.  For others, see MassType for definitions  **NFLOWS
    float TRes;  // Residence time of fuel for this cycle
    struct CycStruct *NextCyc;
  } CYCSTR;

typedef struct OrcaStruct
  {  // Contains everything necessary to derive ORCA ratios
    float TotCost;  // Total cost of case, mills/kWh
    float Variance;  // variance of total cost, (mills/kWh)^2
  } ORCASTR;

typedef struct FuelStruct
  {  // contains all data concerning the cycles, whether reprocessing or once-through
    COSTSTR *Costs, *CostFront;  // Data for the unit costs stored here
    CYCSTR *Cycles, *CycFront;  // Data for the fuel cycles stored here
    float DRate;  // Discount rate
    ORCASTR Orca[2];  // Data used for comparison to base case / ORCA ratio generation
    int ORCAIndex;  // 0=base case, 1=test case
    float ElecGen;  // (Discounted) electricity generated
    float CostDiff, CostDiffVar;  // Difference in cost between base case and test case and its variance
  } FUESTR;

typedef struct Tollstruct
  {   // This structure contains data concerning isotopes to be toll-enriched: up to 5-species toll enrichment
    int nspecies;  // Number of species in feed; typically 3 for uranium
    float MEnrich[3]; // Masses for enrichment: 0 = feed, 1 = tails, 2 = product
    float XEnrich[3][5]; // mol fractions for enrichment, first index as defined above, second index species, typically: 0: 235-U, 1: 236-U, 2:238-U
    float WEnrich[5]; // atomic weights for enrichment, for each species
    float MKey; // 'Key Weight': See De La Garza's paper
    float Stages[3]; // Stripper [1] and enricher [2] stages S_{s} & S_{e}
} TESTR;

typedef struct CompStruct {
  // describes a material component of the reactor
  char Name[30];   // name of the material
  float density;   // density [g/cm^3]
  float volpct;    // v/o material occupies in this region
  struct CompStruct *NextMatl;
} COMPSTR;

typedef struct FrontEndStruct {
  // contains front end mass flow variables
  float Mass[NACT];  // mass of each of 23 actinides into reactor
  float MTotal;      // total mass of fuel batch being tracked
} FRONTSTR;

typedef struct ReactorStruct {
  float volratio;   // volume ratio V_0/V_1
  float r[2];  // relevant dimensions:  sphere: r[0]=pellet radius, r[1]=annular radius as derived from volratio -- slab: r[0]=fuel region thickness, r[1]=coolant region thickness -- cylinder: r[0]=cylinder radius, r[1]=pitch [cm]
  int geomtype;  // 0=spherical, 1=cylindrical, 2=plate
  char Name[30];   // name of the reactor
  float core_r;    // (cylindrical) core radius [cm]
  float core_h;    // (cylindrical) core height [cm]
  float core_r_equiv;  // (spherical) core equivalent radius [cm]
  float db2;       // (homogenized) escape cross section
  int Fission_source_region;  // where does the fuel live?
  float t_clad;    // cladding thickness [cm]
  float T[2];      // temperatures; regions 0 and 1
  int zones[2];    // number of zones in regions 0 and 1
  float power, inventory, effic, load_factor;  // reactor power [MW_t], fuel inventory [kgHM], thermal efficiency [%], load factor [%]
  float pow_den;   // power density [MW_t/kgHM]
  struct ReactorStruct *NextReactor;  // points to next reactor in struct
  COMPSTR *Matl[4], *MatlFront[4];      // points to materials composing regions [0] and [1] of reactor, to control absorbers [2], to structural material [3]
  char CladName[30];  // cladding name e.g. zircaloy
  float clad_rho;   // cladding density [g/cm^3]
  float fuel_density;   // base density of fuel: this depends on matrix choice (oxide, carbide... just "fuel" assmes metallic fuel at natural actinide number density)
} REACSTR;

typedef struct RunStruct {
  // contains all data regarding the run to be completed
  REACSTR *Reactor, *ReactorFront;
  int ncurrent;  // current group #
  float dancoff[NUM_POINTS_LOOKUP][2];  // dancoff[][0]: dancoff factor; dancoff[][1]: sigma_a value: overwritten for each new reactor type, but not important enough to store
  // These are derived for use in multigroup calculations: it's worthwhile to save these for checking flux distributions
  float inscfine[NFAST][2];  // Inscattering source terms for fine groups, regions 0 and 1
  float insc[NFAST/10][2];   // Inscattering source terms
  float insc_th[NTH*2];     // Thermal group inscattering source terms; ordered as [insc_th[0][0] insc_th[0][1] insc_th[1][0] insc_th[1][1]...], for purposes of being able to pass a vector to the LU decomposition routines
  float phifine[NFAST][2];   // fluxes, fine group structure
  float phi[NFAST/10][2];    // fluxes, coarse group structure
  float phi_th[NTH*2];      // thermal fluxes; structured as insc_th
  float th_matrix[NTH*2][NTH*2];   // matrix of transfer functions to be decomposed for solution of thermal fluxes
  //float Fission_sourcefine[NFAST];  // fission source vector; entries should add to 1
  float Fission_source[NFAST/10];    // coarse group fission source vector
  float Source_strength;             // Fission source strength; this number is arbitrary
  int casenum;   // 4-digit case number: so far, I've defined: Nxxx N=0:conventional U-Pu run; xNxx N=number of plutonium recycles; xxNx N=number of uranium recycles; xxxN uranium diluent for MOX: 0=DU, 1=NU, 2=RU, 3=blend variant. special: 0004=equilibrium MOX cycle
  float R_abs[NACT];  // absorption rate for actinides #/s
  float R_fiss[NACT];  // fission rate for actinides #/s
  float one_grp_xs[NACT][2];  // equivalent one group xs for actinides
  float act_nd[NACT];
  float R_neut;  // neutron production rate: should match source_strength for a critical reactor
  float ResTime;  // residence time for this run
  float totabs;
  float absfuelth;
  float absfuelfast;
  float absorbed_th;
  float totfn;
  float fast_fn;
  float totfiss;
  float db2s[NTH];
  float k;
  float fast_escape;
  float escape;
  float ca_abs;
  float thermalized;
  float abs_by_group[NFAST/10+NTH][2];
  float outscatter_by_group[NFAST/10+NTH][2];
  float inscatter_by_group[NTH*2][NTH*2];
  float source_by_group[NFAST/10+NTH][2];
  float td_sixfactors[7][N_COARSE];
  float ***td_rvalues;
  float td_fluxes[NFAST/10+NTH][2][N_COARSE];
  float td_finefluxes[NFAST+NTH][2][N_COARSE];
  float td_energy[NFAST/10+NTH];
  float td_actinides[NACT][N_FINE];
  float coarse_t[N_COARSE][2];
  float fine_t[N_FINE][2];
  float norm_flux[N_FINE][2];

} RUNSTR;

typedef struct RegionStruct {
  // contains all data concerning a nuclide in a given region
  float f_factor[6];  // f-factors: will eventually be read in from data
  float xs; // microscopic total cross section (unshielded); units of barns
  float xs_c[NFAST/10];
  float f_calc;  // calculated f-factor: the main output
  float f_old; // previous values of the f-factor; used for convergence check
  float nd; // number densities; units of 10^24
  int important;  // boolean variable that checks against 2 conditions to determine whether the nuclide: 1) plays a significant neutronics role, 2) undergoes appreciable self-shielding
  float downscatterfine[FINEDS][2];    // Scattering array to be generated; 0 = isotropic, 1=linearly anisotropic -- for all but hydrogen 1, which is treated separately, a collision cannot transfer a neutron by more than 100 fine groups -- hence the indexing
  float downscatter[COARSEDS][2];  // Scattering array for coarse group structure: again H-1 is treated separately
  float sc_krnl[NTH];  // scattering kernel; one vector of matrix
  float inel_krnl[NFAST];   // inelastic scattering kernel; one vector of matrix
  float inel_krnl_c[NFAST/10];  // coarsened inelastic scattering kernel
  float inel_ss;   // selfscattering component of inelastic scattering kernel
  float f_stored[NFAST];    // stored f-factors for group thinning
  float mu_lab;             // average scattering cosine (lab frame) for this nuclide
  float mu_lab_c[NFAST/10];
  float alpha;              // alpha = (A-1)^2/(A+1)^2 for this nuclide derived in a function in this module; should be done up front
  int a;                  // atomic weight of nuclide
  float mu_cm;              // average scattering cosine (cm frame) derived in a function in this module; this only needs to be done once, up front
  float lethrange;        // = ln(1/alpha); precomputed
  int ndownfine, ndown;   // keeps track of the number of nonzero entries in downscattering vectors so we don't waste time sweeping through a bunch of zeroes
  float sigma_i;      // inelastic scattering x-s
  float sigma_i_c[NFAST/10];
  float sigma_s;         // scattering x-s
  float sigma_s_c[NFAST/10];
  float sigma_f;         // fission x-s
  float sigma_f_c[NFAST/10];
  float sigma_g;         // (n,gamma) x-s (+ (n,alpha) in a couple of cases)
  float sigma_g_c[NFAST/10];
  float sigma_tr;        // transport x-s
  float nubar;           // # neutrons/fission
  float nubar_c[NFAST/10];
  float fn_rate_c[NFAST/10];
  float lambda;          // decay constant [1/d]
  float ca_nd_factor;    // control absorber number density factor: only nonzero for control absorbers
  float cum_abs;         // tracks cumulative n's absorbed by this nuclide
  float cum_fiss;        // cumulative fissions induced in this nuclide
  float cum_fn;          // cumulative fission neutrons produced by fissions in this nuclide
  char prn_name[25];
  struct RegionStruct *NextNucl, *PrevNucl;  // pointer to next & previous nuclide in struct
  nonactinide *rawdata;   // if the species is a nonactinide, the raw data is stored here
} REGSTR;  // this struct holds the important stuff for self shielding calculations

typedef struct GroupStruct {
  float ndtot;  // total number densities
  float Pi;     // transmission probabilities
  float Sigma;  // macroscopic total cross section
  float Sigma_tr; // macroscopic transport cross section
  float Sigma_s;  // macroscopic scattering cross section
  float Sigma_g;  // macroscopic (n,g) cross section
  float Sigma_f;  // macroscopic fission cross section
  float Sigma_ss; // macroscopic selfscattering cross section
  float Sigma_i;  // macroscopic inelastic scattering cross section
  float Sigma_sk[NTH];  // macroscopic scattering kernel
  REGSTR *Nuclide, *NuclFront;  // pointers to region data
} GRPSTR;

typedef struct TheCaseStruct {  // data for case being run
  float fiss_rate;  // fissions/sec
  float rho_stored[10];  // for fuel management
  float T_res;   // restime
  float LF;     // load factor
  float burnup;  // burnup MWd/kg
  int reloads;   // # reloads per cycle
  float ihm;     // initial heavy metal in kg
  float uptime;  // cycle uptime in days
  float downtime;  // cycle downtime in days
} CASESTR;

CASESTR TheCase;

GRPSTR Grp[2];  // holds all relevant data for a group, regions 0 and 1

RUNSTR Run;  // holds relevant data for a run; the uber-structure

TESTR Toll;

FUESTR Fuel;  // Contains all data relevant to the economic model cost calculations

FRONTSTR FrontEnd;  // front end masses, swu, etc.

// TOLL prototype definitions
void efunc(int, float[], float[]);
void tollenrich(float);
float calc_a(float[], float);
float flowfunc(float);
void calc_x();

// GET_COST prototype definitions
void Populate();
void ProcessCycle();
void Create_empty_list();
void Insert_costnode(COSTSTR*, COSTSTR*);
void Insert_cycnode(CYCSTR*, CYCSTR*);
COSTSTR *Create_costnode(COSTSTR*);
CYCSTR *Create_cycnode(CYCSTR*);
void Link_costlist(COSTSTR*);
void Link_cyclist(CYCSTR*);
float OneTimeCost(float,float);
float DistribCost(float,float,float);
float OTCostDPayment(float,float,float,float,float);
float DCostDPayment(float,float,float,float,float,float);
float EscDistCost(float,float,float,float);
float EscDistCostDP(float,float,float,float,float,float,float);
float Revenue();
void LoadCyc();
void DeriveCost();
void WipeCosts();
void DeriveCostDiff();

// DOWNSCATTER prototype definitions
float convert_mu(float, int);
void get_alpha(float*,float*,int);
void derive_downscatter(float*,float,int*,int,float,float);
void derive_inscatterfine(float*,float[][2],int,int,float,float,float,float,int);
void derive_inscatter(float*,float*,float[][2],int,int,float,float,float,float,int);
float iso_a(int,float,float,float);
float iso_b(int,float,float,float);
float iso_c(int,float,float,float);
float iso_d(float,float,float);
float aniso_a(int,float,float,float);
float aniso_b(int,float,float,float);
float aniso_c(int,float,float,float);
float aniso_d(float,float,float);

// GET_PROBS prototypes
float get_t0slab(float);
float get_t0cyl(float);
float get_t0sph(float);
float reciprocity(float,float,float);
float get_t1sph(float,float);
float get_t1cyl(float);
void load_dancoff(float);
void interp_dancoff(float);
float get_pi0(float, float, float);
float get_pi1(float, float, float, float);
void derive_volratio(float*, float*, int);
void standard_error(char[]);
float expint(int,float);

// SELFSHIELD prototypes
void get_tot_nd();
void get_probs(float*, float*);
void get_sigmas();
void is_important(float);
float homogenize(float, float, float);
void calc_f_factor();
float interp_f(float, float[]);
float lininterp(float, float, float, float, float);
void Link_nuclreglist(REGSTR*, int);
REGSTR *Create_nuclregnode(REGSTR*);
void Insert_nuclregnode(REGSTR*, REGSTR*);
REGSTR *Create_empty_nuclregnode(REGSTR*);
void Insert_empty_nuclregnode(REGSTR*, REGSTR*);
void Free_nucreglist();
void freeNuclRegList();
void freeRunAndReactors();
void freeSpecificNuclRegStruct(REGSTR*);
float LoadNuclReg();
int backup_f();
void InitializeNuclReg(float*, int);
void TempLoadNuclReg();
int conv_test(int);

// SOLVE_FLUX prototypes
void downscatter_source(REGSTR*);
void augment_inscatterfine();
void augment_inscatter();
void augment_inscatter_inel(float*,float[],int,float,float,float,int,int);
void Link_reaclist(REACSTR*);
REACSTR *Create_reacnode(REACSTR*);
void Insert_reacnode(REACSTR*,REACSTR*);
void selfshield();
void solve_multigroup(float*, float*, float, int, float, float);
void get_sigma_tr();
void update_xs();
void deduct_selfscatter();
float get_diff_coeff(float, float);
float get_buckling(float, float, float);
float buckle_func(float, float, float,float);
void group_thin();
void get_flux();
void get_th_flux();
void get_selfscatter();
void get_selfscatterfine();
void get_selfscatter_th(int);
void GetRawDataFine(int);
void GetRawData(int);
void GetRawDataTh(int);
void update_xs_c();
void update_xs_c_th();
void get_next_inscatter(int);
void augment_abs_fiss();
void augment_abs_fiss_th();
void InitializeThReg();
void get_fine_flux();
void ludcmp(float*, int, int*, float*);
void lubksb(float*, int, int*, float[]);

// FUELCYCLE prototypes
void FindSWU();

// LOAD_NUCLIDES prototypes
void set_all_actinides(float);

// Feb 03 Andy
nonactinide *get_nonactinide(char*,float,char*, nonactinide*);
//void get_nonactinide(char*, float, char*, nonactinide*);

actinide *get_actinide(char*, float);
void interpolate_nonactinide(nonactinide*,char*,float,char*);
void interpolate_actinide(actinide*,char*,float);
void load_bound_nonactinide(nonactinide*,char*,char*,float);
void load_nonactinide(nonactinide*,char*,float);
void load_actinide(actinide*,char*,float);
void load_actinide_matrix(float[][NTH],char*,char*);
void load_actinide_array_lh(float*,char*,char*);
void load_actinide_array_th(float*,char*,char*);
void load_inel(float[][NHI],char*,char*);
void zero_nonactinide(nonactinide*);
void zero_actinide(actinide*);
void load_meshes();
void load_fiss_spec();
void verify_kernel(float[][NTH],float[NTH]);

// LOAD_REACTORS prototypes
void Link_matlist(COMPSTR*,int);
COMPSTR *Create_matnode(COMPSTR*);
void Insert_matnode(COMPSTR*,COMPSTR*);
void echo_reactor_choice();
void put_actinide(REGSTR*,int,int,int);
void assign_actinide(int,REGSTR*);
void put_nonactinide(REGSTR*,int,int);
void assign_nonactinide(REGSTR*);
float clad_vol_frac();
void load_fuel(int,float,float);
void put_actinide_c(REGSTR*,int,int,int);
void put_nonactinide_c(REGSTR*,int,int);
void get_fission_spec_c();
void get_inel_kernel_c();

// TIME_EVOLVE prototypes
void take_timesteps(float,float*);
void take_decaysteps(float);
void step_forward(float*, float*, float*, float);
void decay_step(float*, float);
void update_ca_conc(int,int,float,float,float*);
void set_ca_conc(int,float);
void populate_case();

// MEX prototypes
void VBUDSLoadReactors();
void runVBUDSFromMATLAB(int, int, int, float, float);
void VBUDS_load_fuel(int, float, float, float*);
void VBUDS_populate_case(float, float, int, float);
char **cmatrix(long, long, long, long);

// MAIN prototypes
void LoadReactors();
float get_e(int, float, float);
void assign_fudge();
void economic_module();
int get_case();

FILE *fglobal,*fphi, *fdebug, *ff, *fir, *fxs;

// Jan 19 Andy - free stuff
void freeCmatrix(char**, long);
// Feb 03 Andy - declare 46 nonactinides to avoid memory leaks when *get_nonactinide returns a pointer to
// memory allocated within local scope
nonactinide nonActArray[46];
// Feb 04 Andy - declare NACT REGSTRs to prevent allocation.
REGSTR regionStructArray[NACT];

// Dec 20 2010 Bob - solve function reads and builds .mat
// files outside of matlab.
void solveRun();

void solveRun()
{
    char *filename = "variables_before_vbuds.mat";
    //char *filename = "test-output-imf-01.mat";
    char *filenameEnd = "variables_after_vbuds.mat";
    MATFile *pmat;
    MATFile *lmat;
    mxArray *prhs[34];
    int nrhs =34;
    mxArray *plhs[10];
    int nlhs =10;

    //check declarations
    //printf("variables declared\n");

    pmat = matOpen(filename, "r");

    //open a file
    //printf("file opened\n");

    prhs[0] = matGetVariable(pmat, "rx_name");
    prhs[1] = matGetVariable(pmat, "geom");
    prhs[2] = matGetVariable(pmat, "fuel_region");
    prhs[3] = matGetVariable(pmat, "compp");
    prhs[4] = matGetVariable(pmat, "rhop");
    prhs[5] = matGetVariable(pmat, "vfp");
    prhs[6] = matGetVariable(pmat, "shim");
    prhs[7] = matGetVariable(pmat, "shimconc");
    prhs[8] = matGetVariable(pmat, "actinidesp");
    prhs[9] = matGetVariable(pmat, "clad_comp");
    prhs[10] = matGetVariable(pmat, "clad_rho");
    prhs[11] = matGetVariable(pmat, "struct_comp");
    prhs[12] = matGetVariable(pmat, "struct_rho");
    prhs[13] = matGetVariable(pmat, "struct_vf");
    prhs[14] = matGetVariable(pmat, "temps");
    prhs[15] = matGetVariable(pmat, "dimen");
    prhs[16] = matGetVariable(pmat, "cell_geom");
    prhs[17] = matGetVariable(pmat, "load_factor");
    prhs[18] = matGetVariable(pmat, "fuel_types");
    prhs[19] = matGetVariable(pmat, "nonfuel_types");
    prhs[20] = matGetVariable(pmat, "ca_types");
    prhs[21] = matGetVariable(pmat, "ca_flag");
    prhs[22] = matGetVariable(pmat, "clad_types");
    prhs[23] = matGetVariable(pmat, "struct_types");
    prhs[24] = matGetVariable(pmat, "mode");
    prhs[25] = matGetVariable(pmat, "x_axis_option");
    prhs[26] = matGetVariable(pmat, "y_axis_option");
    prhs[27] = matGetVariable(pmat, "plot_options");
    prhs[28] = matGetVariable(pmat, "disch_bu");
    prhs[29] = matGetVariable(pmat, "t_res");
    prhs[30] = matGetVariable(pmat, "n_batches");
    prhs[31] = matGetVariable(pmat, "load_factor");
    prhs[32] = matGetVariable(pmat, "flux_step");
    prhs[33] = matGetVariable(pmat, "burn_step");


    //read into prhs
    //printf("read into prhs\n");

    matClose(pmat);
    // input file closed
    printf("input file closed\n");

    mexFunction(nlhs, plhs, nrhs, (const mxArray**)prhs);

    printf("cld checkpoint 2\n");

    //Mex ranReactor Name
    printf("mex ran correctly\n");


    // need to now create a new .mat with the plhs information
    lmat = matOpen(filenameEnd, "w");
    //lmat opens
    printf("lmat opens\n");

    matPutVariable(lmat, "vbuds1", plhs[0]);
    matPutVariable(lmat, "vbuds2", plhs[1]);
    matPutVariable(lmat, "vbuds3", plhs[2]);
    matPutVariable(lmat, "vbuds4", plhs[3]);
    matPutVariable(lmat, "vbuds5", plhs[4]);
    printf("vbuds5\n");
    matPutVariable(lmat, "vbuds6", plhs[5]);
    printf("vbuds6\n");
    matPutVariable(lmat, "vbuds7", plhs[6]);
    printf("vbuds7\n");
    matPutVariable(lmat, "vbuds8", plhs[7]);
    printf("vbuds8\n");
    matPutVariable(lmat, "vbuds9", plhs[8]);
    matPutVariable(lmat, "vbuds10", plhs[9]);

    //matPutVariable works
    printf("matPutVariable works\n");

    matClose(lmat);


}


//void debugPerformRun();
/*
void debugPerformRun()
{


    int ndir;
    //char *filename = "outputdeck.mat";
    //char *filename = "uoxinput.mat";
	char *filename = "test-output-imf-01.mat";
    MATFile *pmat;
    const char **dir;
    const char *name;
    printf("works yo\n");
    mxArray *pa;
    int i;
    mxArray *prhs[34];
    int nrhs = 34;
    mxArray *plhs[10];
    int nlhs = 10;


    pmat = matOpen(filename, "r");
    //dir = (const char **)matGetDir(pmat, &ndir);  // this line causing segfault?
    //mxFree(dir);
    matClose(pmat);
    pmat = matOpen(filename, "r");

    prhs[0] = matGetVariable(pmat, "rx_name");
    prhs[1] = matGetVaReactor Nameriable(pmat, "geom");
    prhs[2] = matGetVariable(pmat, "fuel_region");
    prhs[3] = matGetVariable(pmat, "compp");
    prhs[4] = matGetVariable(pmat, "rhop");
    prhs[5] = matGetVariable(pmat, "vfp");
    prhs[6] = matGetVariable(pmat, "shim");
    prhs[7] = matGetVariable(pmat, "shimconc");
    prhs[8] = matGetVariable(pmat, "actinides");
    prhs[9] = matGetVariable(pmat, "clad_comp");
    prhs[10] = matGetVariable(pmat, "clad_rho");
    prhs[11] = matGetVariable(pmat, "struct_comp");
    prhs[12] = matGetVariable(pmat, "struct_rho");
    prhs[13] = matGetVariable(pmat, "struct_vf");
    prhs[14] = matGetVariable(pmat, "temps");
    prhs[15] = matGetVariable(pmat, "dimen");
    prhs[16] = matGetVariable(pmat, "cell_geom");
    prhs[17] = matGetVariable(pmat, "load_facctor");
    prhs[18] = matGetVariable(pmat, "fuel_types");
    prhs[19] = matGetVariable(pmat, "nonfuel_types");
    prhs[20] = matGetVariable(pmat, "ca_types");
    prhs[21] = matGetVariable(pmat, "ca_flag");
    prhs[22] = matGetVariable(pmat, "clad_types");
    prhs[23] = matGetVariable(pmat, "struct_types");
    prhs[24] = matGetVariable(pmat, "mode");
    prhs[25] = matGetVariable(pmat, "x_axis_option");
    prhs[26] = matGetVariable(pmat, "y_axis_option");
    prhs[27] = matGetVariable(pmat, "plot_options");
    prhs[28] = matGetVariable(pmat, "disch_bu");
    prhs[29] = matGetVariable(pmat, "t_res");
    prhs[30] = matGetVariable(pmat, "n_batches");
    prhs[31] = matGetVariable(pmat, "load_factor");
    prhs[32] = matGetVariable(pmat, "flux_step");
    prhs[33] = matGetVariable(pmat, "burn_step");

    matClose(pmat);

    mexFunction(nlhs, plhs, nrhs, (const mxArray**)prhs);

    printf("got here?\n");


    for(i=0;i<34;i++)
        mxDestroyArray(prhs[i]);


    //for(i=0;i<10;i++)
     // mxDestroyArray(plhs[i]);

    /*
    for(i=0;i<34;i++)
        printf("According to its header, array %d has %d dimensions\n",
            i, mxGetNumberOfDimensions(prhs[i]));

    for(i=0;i<ndir;i++)
    {
        pa = matGetNextVariableInfo(pmat, &name);
        printf("According to its header, array %s has %d dimensions\n",
            name, mxGetNumberOfDimensions(pa));
        if (mxIsFromGlobalWS(pa))
            printf("  and was a global variable when saved\n");
        else
            printf("  and was a local variable when saved\n");
        mxDestroyArray(pa);
    }
   //Changed by Bob 12/30/2010
 * //should be a star / here




}
*/

// Dec 27 2010 Bob, changing main to run the solve function
// instead of debugPerformRun()
int main(int argc, char* argv[])
{
    solveRun();
    /*
     * int i;
    for(i=0;i<300;i++)
    {
        solve();
        printf("Run number %d\n", i);
    }
    return(1);
    */
    // cld72@cornell.edu 2012/06/19
    return 1;
}

void mexFunction(int nlhs, mxArray *plhs[],
        int nrhs, const mxArray *prhs[])     {

  REACSTR Reac;  // Dummy that holds data as it's read from file
  int i,j,k,l,m,n;
  COMPSTR Material;
  int buflen,n_fields;
  double *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9, *p10;
  double *q1, *q2, *q3, *q4, *q5, *q6, *q7, *q8, *q9;
  double flux_step, burn_step;
  char *fuel_types[20], *nonfuel_types[30], *ca_types[10], *clad_types[10], *struct_types[10];
  int comp_flags[2][5],shim_flag[2],matl_added[2], ca_type_flag,VBUDS_mode_flag,Xaxis_flag,Yaxis_flag;
  mwSize indices[2];
  mwSize indices3d[3];
  int VBUDS_plots[5],n_batches;
  float dens[2][5], vol_frac[2][5],burnup,t_res,load_factor;
  float actinides_in[NACT];
  float six_factors[7];
  char inel_path[128];
  float VBUDS_fluxes[(NFAST/10+NTH)*2][2];
  float VBUDS_finefluxes[(NFAST+NTH)*2][2];
  float VBUDS_energies[(NFAST/10+NTH)*2];
  float VBUDS_fineenergies[(NFAST+NTH)*2];
  float bound_amu;
  char **r_names;
  float **r_values;
	// keep track of various array sizes so we can free later on.
	int kRecordForFree;
	int nFuelTypes;
	int nNonFuelTypes;
	int nCaTypes;
	int nCladTypes;
	int nStructTypes;




  // Jan 7 2010 Andy Osborne - clear 6-factors before every run.  Subsequent
  // calls to routine from matlab without recompilation in between do not reset
  // state.
  for(i=0;i<7;i++)
      for(j=0;j<N_COARSE;j++)
          Run.td_sixfactors[i][j] = 0;

  // POSSIBLE MODIFICATION: input array PRHS could now describe N (maybe up to 5) reactor (pin-cell) types, rather than just 1

  matl_added[0]=0;
  matl_added[1]=0;
  Run.Reactor=Run.ReactorFront=NULL;
  j=0;
  i=0;


  // IF you had more than one reactor, there would be a loop from here...
  buflen=mxGetM(prhs[0]) * mxGetN(prhs[0]) + 1;
  i=mxGetString(prhs[0],Reac.Name,buflen);
  Reac.Name[buflen-1]='\0';
  p1=mxGetPr(prhs[14]);
  Reac.T[0]=p1[0];
  Reac.T[1]=p1[1];
  // DIRTY LAUNDRY
  if(Reac.T[0] == 300. || Reac.T[0] == 600. || Reac.T[0] == 900. || Reac.T[0] == 1200.) {
      Reac.T[0] += 0.0001;
  }
  if(Reac.T[1] == 300. || Reac.T[1] == 600. || Reac.T[1] == 900. || Reac.T[1] == 1200.) {
      Reac.T[1] += 0.0001;
  }
  if(Reac.T[0] == 1500.) {
      Reac.T[0] -= 0.0001;
  }
  if(Reac.T[1] == 1500.) {
      Reac.T[1] -= 0.0001;
  }
  Reac.load_factor=mxGetScalar(prhs[17]);
  // Thse are not used yet
  Reac.power=1.;
  Reac.inventory=1.;
  Reac.effic=100.;
  Reac.effic=Reac.effic/100.;
  Reac.load_factor=Reac.load_factor/100.;
  Reac.pow_den=Reac.power/Reac.inventory;
  p2=mxGetPr(prhs[16]);
  Reac.r[0]=p2[0];
  Reac.r[1]=p2[1];
  Reac.t_clad=p2[2];
    // r[0] r[1] t_clad CR core_r core_h CR geomtype Fission_region CR
  Reac.r[0]=Reac.r[0]+Reac.t_clad;  // we lump cladding with region 0
  p3=mxGetPr(prhs[15]);
  Reac.core_r=p3[0];
  Reac.core_h=p3[1];
  Reac.core_r_equiv=pow(0.75*Reac.core_r*Reac.core_r*Reac.core_h,1./3.);
  Reac.geomtype=mxGetScalar(prhs[1]);
  Reac.Fission_source_region=mxGetScalar(prhs[2]);
  derive_volratio(Reac.r,&Reac.volratio,Reac.geomtype);

	// looks like j is always 0 here.  Maybe a debugging feature.
  if(j==0) {
    Link_reaclist(&Reac);
    j=1;
  }
  else {
    Insert_reacnode(Run.Reactor,&Reac);
    Run.Reactor=Run.Reactor->NextReactor;
  }
  // down to here

  // maybe you don't to break the loop....
  // Then, there would be another loop to read case data like material inputs, burup cycle time, etc. for more than 1 case...
  nFuelTypes=mxGetNumberOfElements(prhs[18]);
  for(i=0; i<nFuelTypes; i++) {
      fuel_types[i]=mxArrayToString(mxGetCell(prhs[18],i));
  }
  nNonFuelTypes=mxGetNumberOfElements(prhs[19]);
  for(i=0; i<nNonFuelTypes; i++) {
      nonfuel_types[i]=mxArrayToString(mxGetCell(prhs[19],i));
  }
  nCaTypes=mxGetNumberOfElements(prhs[20]);
  for(i=0; i<nCaTypes; i++) {
      ca_types[i]=mxArrayToString(mxGetCell(prhs[20],i));
  }
  nCladTypes=mxGetNumberOfElements(prhs[22]);
  for(i=0; i<nCladTypes; i++) {
      clad_types[i]=mxArrayToString(mxGetCell(prhs[22],i));
  }
  nStructTypes=mxGetNumberOfElements(prhs[23]);
  for(i=0; i<nStructTypes; i++) {
      struct_types[i]=mxArrayToString(mxGetCell(prhs[23],i));
  }
  m=mxGetM(prhs[3]);
  n=mxGetN(prhs[3]);
  p4=mxGetData(prhs[3]);
  p5=mxGetPr(prhs[4]);
  p6=mxGetPr(prhs[5]);
	// first loop over regions?
  for(i=0; i<n; i++) {
	  // then over materials in region?
      for(k=0; k<m; k++) {
          comp_flags[i][k]=p4[i*m+k];
          dens[i][k]=p5[i*m+k];
          vol_frac[i][k]=p6[i*m+k];
          if(comp_flags[i][k] > 0) {
                if(i==Reac.Fission_source_region && k==0) {
                    sprintf(Material.Name,"%s", fuel_types[comp_flags[i][k]-1]);
                } else {
                    sprintf(Material.Name,"%s", nonfuel_types[comp_flags[i][k]-1]);
                }
                Material.volpct=vol_frac[i][k]/100.;
                Material.density=dens[i][k];
                if(matl_added[i] < 1) {
                      Link_matlist(&Material,i);
                      matl_added[i]=1;
                }
                else {
                        Insert_matnode(Run.Reactor->Matl[i],&Material);
                        Run.Reactor->Matl[i]=Run.Reactor->Matl[i]->NextMatl;
                }
          }
      }
      if(matl_added[i] == 0) {
          sprintf(Material.Name,"hydrogen");
          Material.density=0.0;
          Material.volpct=0.;
          Link_matlist(&Material,i);
      }
  }
  p7=mxGetData(prhs[6]);
  for(i=0; i<2; i++) {
      shim_flag[i]=p7[i];
      if(shim_flag[i] > 0) {
          sprintf(Material.Name,"boron");
          Material.volpct=0.;
          Material.density=(mxGetScalar(prhs[7]));
          if(matl_added[i] < 1) {
               Link_matlist(&Material,i);
               matl_added[i]=1;
          }
          else {
               Insert_matnode(Run.Reactor->Matl[i],&Material);
               Run.Reactor->Matl[i]=Run.Reactor->Matl[i]->NextMatl;
          }
      }
  }
  ca_type_flag=mxGetScalar(prhs[21]);
  if(ca_type_flag > 1) {
        sprintf(Material.Name,"%s", ca_types[ca_type_flag-2]);
        Material.density=0.;
        Material.volpct=0.;
        Link_matlist(&Material,2);
  } else {
        sprintf(Material.Name,"%s", ca_types[0]);
        Material.density=0.;
        Material.volpct=0.;
        Link_matlist(&Material,2);
  }
  ca_type_flag=mxGetScalar(prhs[9])-2;
  if(ca_type_flag > -1) {
        sprintf(Run.Reactor->CladName,"%s", clad_types[ca_type_flag]);
        Run.Reactor->clad_rho=mxGetScalar(prhs[10]);
  }
  ca_type_flag=mxGetScalar(prhs[11])-2;
  if(ca_type_flag > -1) {
        sprintf(Material.Name,"%s", struct_types[ca_type_flag]);
        Material.density=mxGetScalar(prhs[12]);
        Material.volpct=mxGetScalar(prhs[13])/100.;
  } else {
        sprintf(Material.Name,"%s", struct_types[0]);
        Material.density=0.;
        Material.volpct=0.;
  }
  Link_matlist(&Material,3);
  sprintf(inel_path,ACTINIDE_PATH"%su-238%s",SEP_CHAR,SEP_CHAR);
  load_inel(globalinel,inel_path,"output.300.6.51-91");
  load_fiss_spec();
  get_fission_spec_c();
  get_inel_kernel_c();
  echo_reactor_choice();
  bound_amu=LoadNuclReg();
  p8=mxGetPr(prhs[8]);
  for(i=0; i<NACT; i++) {
      actinides_in[i]=p8[i];
  }
  VBUDS_load_fuel(Run.Reactor->Fission_source_region,Run.Reactor->fuel_density,bound_amu,actinides_in);
  get_tot_nd();
  VBUDS_mode_flag=mxGetScalar(prhs[24]);
  Xaxis_flag=mxGetScalar(prhs[25]);
  Yaxis_flag=mxGetScalar(prhs[26]);
  p9=mxGetPr(prhs[27]);
  for(i=0; i<5; i++) {
      VBUDS_plots[i]=p9[i];
  }
  burnup=mxGetScalar(prhs[28]);
  t_res=mxGetScalar(prhs[29]);
  n_batches=mxGetScalar(prhs[30]);
  load_factor=mxGetScalar(prhs[31]);
  flux_step=mxGetScalar(prhs[32]);
  burn_step=mxGetScalar(prhs[33]);
  VBUDS_populate_case(burnup,t_res,n_batches,load_factor);
  printf("About to run VBUDS_solve_rx.c...\n");
  if(VBUDS_mode_flag==1) {
      k=0;
      for(i=0; i<2; i++) {
        Grp[i].Nuclide=Grp[i].NuclFront;
        while(Grp[i].Nuclide != NULL) {
          Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
          k++;
        }
      }
      Run.td_rvalues=f3tensor(0, k+1, 0, 3, 0, N_COARSE);
  }
	kRecordForFree = k;
  // This executes the burnup calculation.  You don't need to descend into this for now.
  runVBUDSFromMATLAB(VBUDS_mode_flag,Xaxis_flag,Yaxis_flag,flux_step,burn_step);
  printf("stage 1\n");
  // that loop would end here.
  // the rest is post-processing to return the results to MATLAB.

  if(VBUDS_mode_flag==0) {
      k=0;
      for(i=0; i<2; i++) {
        Grp[i].Nuclide=Grp[i].NuclFront;
        while(Grp[i].Nuclide != NULL) {
          Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
          k++;
        }
      }
      r_values = matrix(0, k+1,0, 3);
      r_names = cmatrix(0, k+1,0, 25);

      plhs[1] = mxCreateDoubleMatrix(k+1,3, mxREAL);
      k=0;
      for(i=0; i<2; i++) {
        Grp[i].Nuclide=Grp[i].NuclFront;
        while(Grp[i].Nuclide != NULL) {
          sprintf(r_names[k],"%s", Grp[i].Nuclide->prn_name);
          r_values[k][0]=Grp[i].Nuclide->cum_abs;
          r_values[k][1]=Grp[i].Nuclide->cum_fiss;
          r_values[k][2]=Grp[i].Nuclide->cum_fn;
          printf("%s %e %e %e \n",r_names[k],r_values[k][0],r_values[k][1],r_values[k][2]);
          k++;
          Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        }
      }
      sprintf(r_names[k],"leakage");
      r_values[k][0]=Run.escape;
      r_values[k][1]=0.;
      r_values[k][2]=0.;
      k++;
      plhs[0] = mxCreateCharMatrixFromStrings(k, (const char**) r_names);
      q1=mxGetPr(plhs[1]);
      for(j=0; j<k; j++) {
        for(l=0; l<3; l++) {
            q1[j+l*k]=r_values[j][l];
          }
      }
      indices[0]=7;
      indices[1]=1;
      plhs[2] = mxCreateNumericArray(2,(const mwSize*) indices, mxDOUBLE_CLASS, mxREAL);
      six_factors[0]=(Run.totfn-Run.fast_fn)/Run.absfuelth;
      six_factors[1]=Run.absfuelth/(Run.absorbed_th);
      six_factors[2]=Run.thermalized/(Run.totabs-Run.fast_escape);
      six_factors[3]=Run.totfn/(Run.totfn-Run.fast_fn);
      six_factors[4]=(Run.totabs-Run.fast_escape)/Run.totabs;
      six_factors[5]=(Run.absorbed_th-(Run.escape-Run.fast_escape))/Run.thermalized;
      six_factors[6]=1.;
      for(i=0; i<6; i++) {
          six_factors[6] = six_factors[6] * six_factors[i];
      }
      q2 = mxGetPr(plhs[2]);
      for(i=0; i<7; i++) {
          q2[i]=six_factors[i];
      }
      for(i=0; i<NFAST; i++) {
          VBUDS_finefluxes[2*i][0]=Run.phifine[i][0]*10;
          VBUDS_finefluxes[2*i+1][0]=Run.phifine[i][0]*10;
          VBUDS_finefluxes[2*i][1]=Run.phifine[i][1]*10;
          VBUDS_finefluxes[2*i+1][1]=Run.phifine[i][1]*10;
          VBUDS_fineenergies[2*i]=get_e(i,10.e6,DELTAL*DELTAFAST);
          VBUDS_fineenergies[2*i+1]=get_e(i+1,10.e6,DELTAL*DELTAFAST);
      }
      for(i=0; i<NFAST/10; i++) {
          VBUDS_fluxes[2*i][0]=Run.phi[i][0];
          VBUDS_fluxes[2*i+1][0]=Run.phi[i][0];
          VBUDS_fluxes[2*i][1]=Run.phi[i][1];
          VBUDS_fluxes[2*i+1][1]=Run.phi[i][1];
          VBUDS_energies[2*i]=get_e(i,10.e6,DELTAL*DELTAFAST*10.);
          VBUDS_energies[2*i+1]=get_e(i+1,10.e6,DELTAL*DELTAFAST*10.);
      }
      for(i=0; i<NTH; i++) {
          VBUDS_finefluxes[NFAST*2+2*i][0]=Run.phi_th[2*i];
          VBUDS_finefluxes[NFAST*2+2*i+1][0]=Run.phi_th[2*i];
          VBUDS_finefluxes[NFAST*2+2*i][1]=Run.phi_th[2*i+1];
          VBUDS_finefluxes[NFAST*2+2*i+1][1]=Run.phi_th[2*i+1];
          VBUDS_fineenergies[NFAST*2+2*i]=get_e(i,1.,DELTAL*DELTAFAST*10.);
          VBUDS_fineenergies[NFAST*2+2*i+1]=get_e(i+1,1.,DELTAL*DELTAFAST*10.);
          VBUDS_fluxes[NFAST/10*2+2*i][0]=Run.phi_th[2*i];
          VBUDS_fluxes[NFAST/10*2+2*i+1][0]=Run.phi_th[2*i];
          VBUDS_fluxes[NFAST/10*2+2*i][1]=Run.phi_th[2*i+1];
          VBUDS_fluxes[NFAST/10*2+2*i+1][1]=Run.phi_th[2*i+1];
          VBUDS_energies[NFAST/10*2+2*i]=get_e(i,1.,DELTAL*DELTAFAST*10.);
          VBUDS_energies[NFAST/10*2+2*i+1]=get_e(i+1,1.,DELTAL*DELTAFAST*10.);
      }
      plhs[3] = mxCreateDoubleMatrix(NFAST/10*2+NTH*2,3, mxREAL);
      plhs[4] = mxCreateDoubleMatrix(NFAST*2+NTH*2,3, mxREAL);
      q3=mxGetPr(plhs[3]);
      q4=mxGetPr(plhs[4]);
      for(i=0; i<(NTH+NFAST/10)*2; i++) {
        q3[i]=VBUDS_fluxes[i][0];
        q3[(NTH+NFAST/10)*2+i]=VBUDS_fluxes[i][1];
        q3[(NTH+NFAST/10)*4+i]=VBUDS_energies[i];
      }
      for(i=0; i<(NTH+NFAST)*2; i++) {
        q4[i]=VBUDS_finefluxes[i][0];
        q4[(NTH+NFAST)*2+i]=VBUDS_finefluxes[i][1];
        q4[(NTH+NFAST)*4+i]=VBUDS_fineenergies[i];
      }
      freeCmatrix(r_names,0);
    printf("cld checkpoint mexFunction 1\n");
  }
  else if(VBUDS_mode_flag == 1) {
    printf("stage 2\n");
    r_names = cmatrix(0, k+1,0, 25);

    m=0;
    for(i=0; i<2; i++) {
      Grp[i].Nuclide=Grp[i].NuclFront;
      while(Grp[i].Nuclide != NULL) {
        sprintf(r_names[m],"%s", Grp[i].Nuclide->prn_name);
        m++;
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
      }
    }
    sprintf(r_names[m],"leakage");
    m++;
    plhs[0] = mxCreateCharMatrixFromStrings(m,(const char**)r_names);
    printf("stage 3\n");
    indices3d[0]=k+1;
    indices3d[1]=3;
    indices3d[2]=N_COARSE;
    printf("stage 4\n");

	  plhs[1] = mxCreateNumericArray(3, indices3d,mxDOUBLE_CLASS, mxREAL);
    printf("stage 5\n");
    //sleep(5);

    q1=mxGetPr(plhs[1]);
    printf("stage 6\n");
    for(j=0; j<k; j++) {
      for(l=0; l<3; l++) {
        for(m=0; m<N_COARSE; m++) {
          q1[j+l*(k+1)+m*3*(k+1)]=Run.td_rvalues[j][l][m];
        }
      }
    }

    plhs[2]=mxCreateDoubleMatrix(7,N_COARSE,mxREAL);
    q2=mxGetPr(plhs[2]);
    for(l=0; l<7; l++) {
      for(m=0; m<N_COARSE; m++) {
        q2[l+m*7]=Run.td_sixfactors[l][m];
      }
    }
    indices3d[0]=(NFAST/10+NTH)*2;
    indices3d[1]=2;
    indices3d[2]=N_COARSE;
    plhs[3]=mxCreateNumericArray(3, indices3d,mxDOUBLE_CLASS, mxREAL);
    q3=mxGetPr(plhs[3]);
    indices[0]=(NFAST/10+NTH)*2;
    indices[1]=1;
    plhs[4]=mxCreateNumericArray(2, indices,mxDOUBLE_CLASS, mxREAL);
    q4=mxGetPr(plhs[4]);
    for(i=0; i<NFAST/10; i++) {
      VBUDS_energies[2*i]=get_e(i,10.e6,DELTAL*DELTAFAST*10.);
      VBUDS_energies[2*i+1]=get_e(i+1,10.e6,DELTAL*DELTAFAST*10.);
    }
    for(i=0; i<NTH; i++) {
      VBUDS_energies[NFAST/10*2+2*i]=get_e(i,1.,DELTAL*DELTAFAST*10.);
      VBUDS_energies[NFAST/10*2+2*i+1]=get_e(i+1,1.,DELTAL*DELTAFAST*10.);
    }
    for(i=0; i<(NFAST/10+NTH); i++) {
      for(j=0; j<N_COARSE; j++) {
        q3[2*i+j*4*(NFAST/10+NTH)]=Run.td_fluxes[i][0][j];
        q3[2*i+1+j*4*(NFAST/10+NTH)]=Run.td_fluxes[i][0][j];
        q3[2*i+2*(NFAST/10+NTH)+j*4*(NFAST/10+NTH)]=Run.td_fluxes[i][1][j];
        q3[2*i+1+2*(NFAST/10+NTH)+j*4*(NFAST/10+NTH)]=Run.td_fluxes[i][1][j];
      }
      q4[2*i]=VBUDS_energies[2*i];
      q4[2*i+1]=VBUDS_energies[2*i+1];
    }

    indices3d[0]=(NFAST+NTH)*2;
    indices3d[1]=2;
    indices3d[2]=N_COARSE;
    plhs[8]=mxCreateNumericArray(3, (const mwSize *)indices3d,mxDOUBLE_CLASS, mxREAL);
    q8=mxGetPr(plhs[8]);
    indices[0]=(NFAST+NTH)*2;
    indices[1]=1;
    plhs[9]=mxCreateNumericArray(2, (const mwSize *)indices,mxDOUBLE_CLASS, mxREAL);
    q9=mxGetPr(plhs[9]);
    for(i=0; i<NFAST; i++) {
      VBUDS_fineenergies[2*i]=get_e(i,10.e6,DELTAL*DELTAFAST);
      VBUDS_fineenergies[2*i+1]=get_e(i+1,10.e6,DELTAL*DELTAFAST);
    }
    for(i=0; i<NTH; i++) {
      VBUDS_fineenergies[NFAST*2+2*i]=get_e(i,1.,DELTAL*DELTAFAST*10.);
      VBUDS_fineenergies[NFAST*2+2*i+1]=get_e(i+1,1.,DELTAL*DELTAFAST*10.);
    }
    for(i=0; i<(NFAST+NTH); i++) {
      for(j=0; j<N_COARSE; j++) {
        q8[2*i+j*4*(NFAST+NTH)]=Run.td_finefluxes[i][0][j];
        q8[2*i+1+j*4*(NFAST+NTH)]=Run.td_finefluxes[i][0][j];
        q8[2*i+2*(NFAST+NTH)+j*4*(NFAST+NTH)]=Run.td_finefluxes[i][1][j];
        q8[2*i+1+2*(NFAST+NTH)+j*4*(NFAST+NTH)]=Run.td_finefluxes[i][1][j];
      }
      q9[2*i]=VBUDS_fineenergies[2*i];
      q9[2*i+1]=VBUDS_fineenergies[2*i+1];
    }

    indices[0]=N_COARSE;
    indices[1]=2;
    plhs[5]=mxCreateNumericArray(2, (const mwSize *)indices,mxDOUBLE_CLASS, mxREAL);
    q5=mxGetPr(plhs[5]);
    indices[0]=N_FINE;
    plhs[6]=mxCreateNumericArray(2, (const mwSize *)indices,mxDOUBLE_CLASS, mxREAL);
    q6=mxGetPr(plhs[6]);
    for(i=0; i<N_COARSE; i++) {
        q5[i]=Run.coarse_t[i][0];
        q5[i+N_COARSE]=Run.coarse_t[i][1];
    }
    for(i=0; i<N_FINE; i++) {
        q6[i]=Run.fine_t[i][0];
        q6[i+N_FINE]=Run.fine_t[i][1];
    }
    indices[0]=NACT;
    indices[1]=N_FINE;
    plhs[7]=mxCreateNumericArray(2, (const mwSize *)indices,mxDOUBLE_CLASS, mxREAL);
    q7=mxGetPr(plhs[7]);
    for(i=0; i<NACT; i++) {
      for(j=0; j<N_FINE; j++) {
        q7[i+j*NACT]=Run.td_actinides[i][j];
      }
    }
    freeCmatrix(r_names,0);
  }



  printf("cld checkpoint mexFunction 2\n");

    // SEGFAULT IS HAPPENING HERE!!!
    // CHRIS IS NOT FREEING THESE VARS!
    // cld72@cornell.edu 2012/06/19: In chasing a segfault, I stopped the
    // freeing of the f3tensor
	//free_f3tensor(Run.td_rvalues, 0, kRecordForFree+1,0,3,0,N_COARSE);
	  printf("cld checkpoint mexFunction 2.1\n");
	freeNuclRegList();
	  printf("cld checkpoint mexFunction 2.2\n");
	freeRunAndReactors();

  printf("cld checkpoint mexFunction 3\n");

	for(i=0; i<nFuelTypes; i++)
		mxFree(fuel_types[i]);
  printf("cld checkpoint mexFunction 4\n");
	for(i=0; i<nNonFuelTypes; i++)
		mxFree(nonfuel_types[i]);
  printf("cld checkpoint mexFunction 5\n");
	for(i=0; i<nCaTypes; i++)
		mxFree(ca_types[i]);
  printf("cld checkpoint mexFunction 6\n");
	for(i=0; i<nCladTypes; i++)
		mxFree(clad_types[i]);
  printf("cld checkpoint mexFunction 7\n");
	for(i=0; i<nStructTypes; i++)
		mxFree(struct_types[i]);
  printf("cld checkpoint mexFunction 8\n");

  //sleep(5);

}


void runVBUDSFromMATLAB(int mode, int x_flag, int y_flag, float flux_step, float burn_step) {


  FILE *f;
  int i,j,k,nits,iter_counter={0},cycle_num;
  int outer_counter={0}, inner_counter={0};
  float sum; // sum of the k's for ca iteration
  float tnow={0.},dt,outer_dt,targ_time,dt_use;
  float cum_bu={0.},sum_1,sum_2,sum_f,next_downtime;
  char  flux_pathname[256];

  cycle_num=1;
  Run.Source_strength=4.e13;
  Run.ResTime=30;
  dt=burn_step;
  dt_use=0.;
  outer_dt=flux_step;
  Run.Reactor=Run.ReactorFront;
  if (Run.Reactor->geomtype == 1) load_dancoff((Run.Reactor->r[0]-Run.Reactor->t_clad)/Run.Reactor->r[1]);
  if(Run.ResTime > 0.0001) {
    Run.ResTime=TheCase.T_res+(TheCase.reloads-1.)*TheCase.downtime;
    next_downtime=TheCase.T_res/TheCase.reloads;
  }
  set_ca_conc(Run.Reactor->Fission_source_region,0.);
  sum_2=1.;
  Grp[Run.Reactor->Fission_source_region].Nuclide=Grp[Run.Reactor->Fission_source_region].NuclFront;
  for(i=0; i<NACT; i++) {
    Run.act_nd[i]=Grp[Run.Reactor->Fission_source_region].Nuclide->nd;
    Grp[Run.Reactor->Fission_source_region].Nuclide=Grp[Run.Reactor->Fission_source_region].Nuclide->NextNucl;
  }
  Grp[Run.Reactor->Fission_source_region].Nuclide=Grp[Run.Reactor->Fission_source_region].NuclFront;
  if(Run.ResTime <= 0.0001) {
    tnow = 1.;
  }
  while(tnow < Run.ResTime) {

    InitializeThReg();
    sum=0.;
    nits=0;
//    printf("\n Flux calculation at time: %f",tnow);
    get_fine_flux();
    group_thin();
    get_flux();  // now we have the flux & the thermal source.
    get_th_flux();

    // populate VBUDS -> MATLAB output

    Run.td_sixfactors[0][outer_counter]=(Run.totfn-Run.fast_fn)/Run.absfuelth;
    Run.td_sixfactors[1][outer_counter]=Run.absfuelth/(Run.absorbed_th);
    Run.td_sixfactors[2][outer_counter]=Run.thermalized/(Run.totabs-Run.fast_escape);
    Run.td_sixfactors[3][outer_counter]=Run.totfn/(Run.totfn-Run.fast_fn);
    Run.td_sixfactors[4][outer_counter]=(Run.totabs-Run.fast_escape)/Run.totabs;
    Run.td_sixfactors[5][outer_counter]=(Run.absorbed_th-(Run.escape-Run.fast_escape))/Run.thermalized;
    Run.td_sixfactors[6][outer_counter]=1.;
    for(i=0; i<6; i++) {
      Run.td_sixfactors[6][outer_counter] = Run.td_sixfactors[6][outer_counter] * Run.td_sixfactors[i][outer_counter];
    }
    sum_f=0;
    for(j=0; j<NFAST/10; j++) {
      sum_f+=Run.phi[j][0];
    }
    for(j=0;j<NTH;j++) {
      sum_f+=Run.phi_th[2*j];
    }

    k=0;
    for(i=0; i<2; i++) {
      Grp[i].Nuclide=Grp[i].NuclFront;
      while(Grp[i].Nuclide != NULL) {
        Run.td_rvalues[k][0][outer_counter]=Grp[i].Nuclide->cum_abs;
        Run.td_rvalues[k][1][outer_counter]=Grp[i].Nuclide->cum_fiss;
        Run.td_rvalues[k][2][outer_counter]=Grp[i].Nuclide->cum_fn;
        k++;
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
      }
    }
    Run.td_rvalues[k][0][outer_counter]=Run.escape;
    Run.td_rvalues[k][1][outer_counter]=0.;
    Run.td_rvalues[k][2][outer_counter]=0.;
    for(i=0; i<NFAST; i++) {
      Run.td_finefluxes[i][0][outer_counter]=Run.phifine[i][0]*10;
      Run.td_finefluxes[i][1][outer_counter]=Run.phifine[i][1]*10;
    }

    for(i=0; i<NFAST/10; i++) {
      Run.td_fluxes[i][0][outer_counter]=Run.phi[i][0];
      Run.td_fluxes[i][1][outer_counter]=Run.phi[i][1];
    }
    for(i=0; i<NTH; i++) {
      Run.td_fluxes[NFAST/10+i][0][outer_counter]=Run.phi_th[2*i];
      Run.td_fluxes[NFAST/10+i][1][outer_counter]=Run.phi_th[2*i+1];
      Run.td_finefluxes[NFAST+i][0][outer_counter]=Run.phi_th[2*i];
      Run.td_finefluxes[NFAST+i][1][outer_counter]=Run.phi_th[2*i+1];
    }

    Run.coarse_t[outer_counter][0]=tnow;
    Run.coarse_t[outer_counter][1]=cum_bu;
    outer_counter++;

    i=0;
    sum_1=0.;

    Grp[0].Nuclide=Grp[0].NuclFront;
    while(i<NACT) {
      sum_1+=Run.R_fiss[i]*Grp[0].Nuclide->nd*Run.Reactor->volratio;
      Run.td_actinides[i][inner_counter]=Grp[0].Nuclide->nd;
      Grp[0].Nuclide=Grp[0].Nuclide->NextNucl;
      Run.one_grp_xs[i][0]=Run.one_grp_xs[i][0]/sum_f;
      Run.one_grp_xs[i][1]=Run.one_grp_xs[i][1]/sum_f;
      i++;
    }

    i=0;
  	sum_2=0.;
  	Grp[0].Nuclide=Grp[0].NuclFront;
  	while(i<NACT) {
    	sum_2=sum_2+Run.R_fiss[i]*Grp[0].Nuclide->nd;
    	Grp[0].Nuclide=Grp[0].Nuclide->NextNucl;
    	i++;
  	}

    Run.fine_t[inner_counter][0]=tnow;
    Run.fine_t[inner_counter][1]=cum_bu;
    Run.norm_flux[inner_counter][0]=sum_f*TheCase.fiss_rate/Run.totfiss*Run.Reactor->volratio;
    Run.norm_flux[inner_counter][1]=Run.R_fiss[5]*TheCase.fiss_rate/sum_2/Run.one_grp_xs[5][1];
    inner_counter++;



    iter_counter++;
    //printf("\n TNOW: ");
    dt_use=dt;
    if(tnow+outer_dt <= next_downtime) {
      targ_time = tnow+outer_dt;
    }
    else {
      targ_time = next_downtime;
    }
    while(tnow < targ_time) {
      if(tnow+dt <= Run.ResTime) {
        take_timesteps(dt,&TheCase.fiss_rate);
        //printf(": %f :",tnow);
        tnow+=dt;
      }
      else {
        take_timesteps(Run.ResTime-tnow,&TheCase.fiss_rate);
        tnow=Run.ResTime;
        dt_use = Run.ResTime-tnow;
      }
      i=0;
      sum_1=0.;

      Grp[0].Nuclide=Grp[0].NuclFront;
      while(i<NACT) {
        sum_1+=Run.R_fiss[i]*Grp[0].Nuclide->nd*Run.Reactor->volratio;
        Run.td_actinides[i][inner_counter]=Grp[0].Nuclide->nd;
        Grp[0].Nuclide=Grp[0].Nuclide->NextNucl;
        i++;
      }
      sum_1 *= TheCase.fiss_rate/Run.totfiss;
      cum_bu+=sum_1*3.2e-11/1e6/(TheCase.ihm)*dt_use;

      i=0;
  		sum_2=0.;
  		Grp[0].Nuclide=Grp[0].NuclFront;
  		while(i<NACT) {
    		sum_2=sum_2+Run.R_fiss[i]*Grp[0].Nuclide->nd;
    		Grp[0].Nuclide=Grp[0].Nuclide->NextNucl;
    		i++;
  		}


      Run.fine_t[inner_counter][0]=tnow;
      Run.fine_t[inner_counter][1]=cum_bu;
      Run.norm_flux[inner_counter][0]=sum_f*TheCase.fiss_rate/Run.totfiss*Run.Reactor->volratio;
      Run.norm_flux[inner_counter][1]=Run.R_fiss[5]*TheCase.fiss_rate/sum_2/Run.one_grp_xs[5][1];
      inner_counter++;

    }
    //printf("\n *** TNOW: day %f  \n",tnow);

  i=0;
  sum_1=0.;
  sum_2=0.;
  Grp[0].Nuclide=Grp[0].NuclFront;
  while(i<NACT) {
    sum_1+=Run.R_fiss[i]*Grp[0].Nuclide->nd*Run.Reactor->volratio;
    sum_2+=Grp[0].Nuclide->nd;
    Grp[0].Nuclide=Grp[0].Nuclide->NextNucl;
    i++;
  }
  sum_2+=Grp[0].Nuclide->nd/2.;
  sum_1 *= TheCase.fiss_rate/Run.totfiss;

  /// DOWNTIME

    if(targ_time >= next_downtime && targ_time < Run.ResTime) {

      targ_time = next_downtime + TheCase.downtime;
      tnow = next_downtime;
      next_downtime = targ_time + TheCase.uptime;
      printf("\n *** DOWNTIME at day %f EXTENDING to day %f  \n",tnow,targ_time);
      cycle_num++;
      if(cycle_num == TheCase.reloads ) {
      	next_downtime = Run.ResTime;
      }

      InitializeThReg();
      sum=0.;
      nits=0;
      get_fine_flux();
      group_thin();
      get_flux();  // now we have the flux & the thermal source.
      get_th_flux();

     // populate VBUDS -> MATLAB output

    Run.td_sixfactors[0][outer_counter]=(Run.totfn-Run.fast_fn)/Run.absfuelth;
    Run.td_sixfactors[1][outer_counter]=Run.absfuelth/(Run.absorbed_th);
    Run.td_sixfactors[2][outer_counter]=Run.thermalized/(Run.totabs-Run.fast_escape);
    Run.td_sixfactors[3][outer_counter]=Run.totfn/(Run.totfn-Run.fast_fn);
    Run.td_sixfactors[4][outer_counter]=(Run.totabs-Run.fast_escape)/Run.totabs;
    Run.td_sixfactors[5][outer_counter]=(Run.absorbed_th-(Run.escape-Run.fast_escape))/Run.thermalized;
    Run.td_sixfactors[6][outer_counter]=1.;
    for(i=0; i<6; i++) {
      Run.td_sixfactors[6][outer_counter] = Run.td_sixfactors[6][outer_counter] * Run.td_sixfactors[i][outer_counter];
    }
    sum_f=0;
    for(j=0; j<NFAST/10; j++) {
      sum_f+=Run.phi[j][0];
    }
    for(j=0;j<NTH;j++) {
      sum_f+=Run.phi_th[2*j];
    }

    k=0;
    for(i=0; i<2; i++) {
      Grp[i].Nuclide=Grp[i].NuclFront;
      while(Grp[i].Nuclide != NULL) {
        Run.td_rvalues[k][0][outer_counter]=Grp[i].Nuclide->cum_abs;
        Run.td_rvalues[k][1][outer_counter]=Grp[i].Nuclide->cum_fiss;
        Run.td_rvalues[k][2][outer_counter]=Grp[i].Nuclide->cum_fn;
        k++;
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
      }
    }
    Run.td_rvalues[k][0][outer_counter]=Run.escape;
    Run.td_rvalues[k][1][outer_counter]=0.;
    Run.td_rvalues[k][2][outer_counter]=0.;
    for(i=0; i<NFAST; i++) {
      Run.td_finefluxes[i][0][outer_counter]=Run.phifine[i][0]*10;
      Run.td_finefluxes[i][1][outer_counter]=Run.phifine[i][1]*10;
    }
    for(i=0; i<NFAST/10; i++) {
      Run.td_fluxes[i][0][outer_counter]=Run.phi[i][0];
      Run.td_fluxes[i][1][outer_counter]=Run.phi[i][1];
    }
    for(i=0; i<NTH; i++) {
      Run.td_fluxes[NFAST/10+i][0][outer_counter]=Run.phi_th[2*i];
      Run.td_fluxes[NFAST/10+i][1][outer_counter]=Run.phi_th[2*i+1];
      Run.td_finefluxes[NFAST+i][0][outer_counter]=Run.phi_th[2*i];
      Run.td_finefluxes[NFAST+i][1][outer_counter]=Run.phi_th[2*i+1];
    }

    Run.coarse_t[outer_counter][0]=tnow;
    Run.coarse_t[outer_counter][1]=cum_bu;
    outer_counter++;

    i=0;
    sum_1=0.;

    Grp[0].Nuclide=Grp[0].NuclFront;
    while(i<NACT) {
      sum_1+=Run.R_fiss[i]*Grp[0].Nuclide->nd*Run.Reactor->volratio;
      Run.td_actinides[i][inner_counter]=Grp[0].Nuclide->nd;
      Run.one_grp_xs[i][0]=Run.one_grp_xs[i][0]/sum_f;
      Run.one_grp_xs[i][1]=Run.one_grp_xs[i][1]/sum_f;
      Grp[0].Nuclide=Grp[0].Nuclide->NextNucl;
      i++;
    }

    i=0;
  	sum_2=0.;
  	Grp[0].Nuclide=Grp[0].NuclFront;
  	while(i<NACT) {
    	sum_2=sum_2+Run.R_fiss[i]*Grp[0].Nuclide->nd;
    	Grp[0].Nuclide=Grp[0].Nuclide->NextNucl;
    	i++;
  	}

    Run.fine_t[inner_counter][0]=tnow;
    Run.fine_t[inner_counter][1]=cum_bu;
    Run.norm_flux[inner_counter][0]=sum_f*TheCase.fiss_rate/Run.totfiss*Run.Reactor->volratio;
    Run.norm_flux[inner_counter][1]=Run.R_fiss[5]*TheCase.fiss_rate/sum_2/Run.one_grp_xs[5][1];
    inner_counter++;


      while(tnow < targ_time) {
         if(tnow+dt <= targ_time) {
            take_decaysteps(dt);
            printf(": %f :",tnow);
            tnow+=dt;
         }
         else {
            take_decaysteps(targ_time-tnow);
            tnow=targ_time;
            dt_use = targ_time-tnow;
         }
         i=0;
         Grp[Run.Reactor->Fission_source_region].Nuclide=Grp[Run.Reactor->Fission_source_region].NuclFront;
         while(i<NACT) {
           Run.td_actinides[i][inner_counter]=Grp[Run.Reactor->Fission_source_region].Nuclide->nd;
           Grp[Run.Reactor->Fission_source_region].Nuclide=Grp[Run.Reactor->Fission_source_region].Nuclide->NextNucl;
           i++;
         }
    		 i=0;
  			 sum_2=0.;
  			 Grp[0].Nuclide=Grp[0].NuclFront;
  			 while(i<NACT) {
    			 sum_2=sum_2+Run.R_fiss[i]*Grp[0].Nuclide->nd;
    			 Grp[0].Nuclide=Grp[0].Nuclide->NextNucl;
    			 i++;
  			 }

         Run.fine_t[inner_counter][0]=tnow;
         Run.fine_t[inner_counter][1]=cum_bu;
         Run.norm_flux[inner_counter][0]=sum_f*TheCase.fiss_rate/Run.totfiss*Run.Reactor->volratio;
      	 Run.norm_flux[inner_counter][1]=Run.R_fiss[5]*TheCase.fiss_rate/sum_2/Run.one_grp_xs[5][1];
         inner_counter++;
      }
    }   /// END  DOWNTIME
  }
    printf("Beginning final spectrum calculation... \n");
    InitializeThReg();
    sum=0.;
    nits=0;
    get_fine_flux();
    group_thin();
    get_flux();  // now we have the flux & the thermal source.
    get_th_flux();
    if(mode==1) {
            // populate VBUDS -> MATLAB output

    Run.td_sixfactors[0][outer_counter]=(Run.totfn-Run.fast_fn)/Run.absfuelth;
    Run.td_sixfactors[1][outer_counter]=Run.absfuelth/(Run.absorbed_th);
    Run.td_sixfactors[2][outer_counter]=Run.thermalized/(Run.totabs-Run.fast_escape);
    Run.td_sixfactors[3][outer_counter]=Run.totfn/(Run.totfn-Run.fast_fn);
    Run.td_sixfactors[4][outer_counter]=(Run.totabs-Run.fast_escape)/Run.totabs;
    Run.td_sixfactors[5][outer_counter]=(Run.absorbed_th-(Run.escape-Run.fast_escape))/Run.thermalized;
    Run.td_sixfactors[6][outer_counter]=1.;
    for(i=0; i<6; i++) {
      Run.td_sixfactors[6][outer_counter] = Run.td_sixfactors[6][outer_counter] * Run.td_sixfactors[i][outer_counter];
    }
    sum_f=0;
    for(j=0; j<NFAST/10; j++) {
      sum_f+=Run.phi[j][0];
    }
    for(j=0;j<NTH;j++) {
      sum_f+=Run.phi_th[2*j];
    }

    k=0;
    for(i=0; i<2; i++) {
      Grp[i].Nuclide=Grp[i].NuclFront;
      while(Grp[i].Nuclide != NULL) {
        Run.td_rvalues[k][0][outer_counter]=Grp[i].Nuclide->cum_abs;
        Run.td_rvalues[k][1][outer_counter]=Grp[i].Nuclide->cum_fiss;
        Run.td_rvalues[k][2][outer_counter]=Grp[i].Nuclide->cum_fn;
        k++;
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
      }
    }
    Run.td_rvalues[k][0][outer_counter]=Run.escape;
    Run.td_rvalues[k][1][outer_counter]=0.;
    Run.td_rvalues[k][2][outer_counter]=0.;
    for(i=0; i<NFAST; i++) {
      Run.td_finefluxes[i][0][outer_counter]=Run.phifine[i][0]*10;
      Run.td_finefluxes[i][1][outer_counter]=Run.phifine[i][1]*10;
    }
    for(i=0; i<NFAST/10; i++) {
      Run.td_fluxes[i][0][outer_counter]=Run.phi[i][0];
      Run.td_fluxes[i][1][outer_counter]=Run.phi[i][1];
    }
    for(i=0; i<NTH; i++) {
      Run.td_fluxes[NFAST/10+i][0][outer_counter]=Run.phi_th[2*i];
      Run.td_fluxes[NFAST/10+i][1][outer_counter]=Run.phi_th[2*i+1];
      Run.td_finefluxes[NFAST+i][0][outer_counter]=Run.phi_th[2*i];
      Run.td_finefluxes[NFAST+i][1][outer_counter]=Run.phi_th[2*i+1];
    }

    Run.coarse_t[outer_counter][0]=tnow;
    Run.coarse_t[outer_counter][1]=cum_bu;
    outer_counter++;

    i=0;
    sum_1=0.;

    Grp[0].Nuclide=Grp[0].NuclFront;
    while(i<NACT) {
      sum_1+=Run.R_fiss[i]*Grp[0].Nuclide->nd*Run.Reactor->volratio;
      Run.td_actinides[i][inner_counter]=Grp[0].Nuclide->nd;
      Run.one_grp_xs[i][0]=Run.one_grp_xs[i][0]/sum_f;
      Run.one_grp_xs[i][1]=Run.one_grp_xs[i][1]/sum_f;
      Grp[0].Nuclide=Grp[0].Nuclide->NextNucl;
      i++;
    }

    i=0;
  	sum_2=0.;
  	Grp[0].Nuclide=Grp[0].NuclFront;
  	while(i<NACT) {
    	sum_2=sum_2+Run.R_fiss[i]*Grp[0].Nuclide->nd;
    	Grp[0].Nuclide=Grp[0].Nuclide->NextNucl;
    	i++;
  	}


    Run.fine_t[inner_counter][0]=tnow;
    Run.fine_t[inner_counter][1]=cum_bu;
    Run.norm_flux[inner_counter][0]=sum_f*TheCase.fiss_rate/Run.totfiss*Run.Reactor->volratio;
    Run.norm_flux[inner_counter][1]=Run.R_fiss[5]*TheCase.fiss_rate/sum_2/Run.one_grp_xs[5][1];
    inner_counter++;

    sum_1 *= TheCase.fiss_rate/Run.totfiss;
    cum_bu+=sum_1*3.2e-11/1e6/(TheCase.ihm)*dt_use;

    for(i=inner_counter; i<N_FINE; i++) {
        Run.fine_t[i][0]=-1;
    }
    for(i=0; i<=inner_counter; i++){
    	//printf("Flux %e %e \n",Run.fine_t[i][0],Run.norm_flux[i][1]);
  	}
    }
    printf("Exiting VBUDS_solve_rx.c...\n");
}

void assign_fudge() {
  // assigns fudge factors to be applied to xs data: primarily for sensitivity analysis

  int i,j;
  for(i=0; i<NACT; i++) {
    for(j=0; j<2; j++) {
      if(i != I_U_238) actinide_fudge[i][j]=1.;
      else actinide_fudge[i][j]=0.87;
    }
  }
}

void VBUDS_populate_case(float burnup, float t_res, int n_batches, float load_factor) {

  // gets fission rate from burnup, establishes refueling schedule and res time
  int i;
  float n_actinide;

  n_actinide=0.;
  TheCase.burnup=burnup;   // MWd/kg
  TheCase.T_res=t_res*n_batches;   // efpd
  TheCase.reloads=n_batches;
  TheCase.LF=load_factor/100.;
  TheCase.downtime=TheCase.T_res/TheCase.reloads*(1.-TheCase.LF)/TheCase.LF;
  TheCase.uptime=TheCase.T_res/TheCase.reloads;
  TheCase.fiss_rate=TheCase.burnup/TheCase.T_res;   // MW/kg
  Grp[Run.Reactor->Fission_source_region].Nuclide=Grp[Run.Reactor->Fission_source_region].NuclFront;
  for(i=0; i<NACT; i++) {
    n_actinide+=Grp[Run.Reactor->Fission_source_region].Nuclide->nd;
    Grp[Run.Reactor->Fission_source_region].Nuclide=Grp[Run.Reactor->Fission_source_region].Nuclide->NextNucl;
  }
  TheCase.ihm=n_actinide*10./6.022*.238;
  TheCase.fiss_rate=TheCase.fiss_rate*TheCase.ihm;  // MJ/s
  TheCase.fiss_rate=TheCase.fiss_rate/3.20e-17;   // MJ/s*(fission/MJ) where 3.2e-11 J or 3.2e-17 MJ is released per fission
}

void populate_case() {

  // gets fission rate from burnup, establishes refueling schedule and res time

  TheCase.burnup=40.; // MWd/kg
  TheCase.T_res=1000.; // efpd
  TheCase.reloads=4;
  TheCase.LF=1.0;
  TheCase.downtime=TheCase.T_res/TheCase.reloads*(1.-TheCase.LF)/TheCase.LF;
  TheCase.uptime=TheCase.T_res/TheCase.reloads;
  TheCase.fiss_rate=TheCase.burnup/TheCase.T_res;   // MW/kg
  TheCase.ihm=Run.Reactor->fuel_density/1000.;//*Run.Reactor->volratio;
  TheCase.fiss_rate=TheCase.fiss_rate*TheCase.ihm;  // MJ/s
  TheCase.fiss_rate=TheCase.fiss_rate/3.20e-17;   // MJ/s*(fission/MJ) where 3.2e-11 J or 3.2e-17 MJ is released per fission
}

void take_decaysteps(float tstep) {

  // framework for calling the timesetep routine at zero power (decay only)

  int i;
  const float dt=0.1;
  float t_now={0.};
  float sum;

  i=0;
  sum=0.;
  Grp[0].Nuclide=Grp[0].NuclFront;

  Grp[Run.Reactor->Fission_source_region].Nuclide=Grp[Run.Reactor->Fission_source_region].NuclFront;
  for(i=0; i<NACT; i++) {
    Run.act_nd[i]=Grp[Run.Reactor->Fission_source_region].Nuclide->nd;
    Grp[Run.Reactor->Fission_source_region].Nuclide=Grp[Run.Reactor->Fission_source_region].Nuclide->NextNucl;
  }
  while (t_now <= tstep-dt) {  // step foward till we reach time=tstep
    decay_step(Run.act_nd,dt);
    t_now=t_now+dt;
  }
  decay_step(Run.act_nd,tstep-t_now);

  Grp[Run.Reactor->Fission_source_region].Nuclide=Grp[Run.Reactor->Fission_source_region].NuclFront;
  for(i=0; i<NACT; i++) {
    Grp[Run.Reactor->Fission_source_region].Nuclide->nd=Run.act_nd[i];
    Grp[Run.Reactor->Fission_source_region].Nuclide=Grp[Run.Reactor->Fission_source_region].Nuclide->NextNucl;
  }

}

void take_timesteps(float tstep, float *total_fissions) {

  // framework for calling the timesetep routine: tstep is the interval thru which we advance (usu. 5 days)

  int i;
  const float dt=0.1;
  float t_now={0.};
  float sum,act_sum;

  i=0;
  sum=0.;
  Grp[0].Nuclide=Grp[0].NuclFront;
  while(i<NACT) {
    sum=sum+Run.R_fiss[i]*Grp[0].Nuclide->nd;
    Grp[0].Nuclide=Grp[0].Nuclide->NextNucl;
    i++;
  }
  Grp[Run.Reactor->Fission_source_region].Nuclide=Grp[Run.Reactor->Fission_source_region].NuclFront;
  for(i=0; i<NACT; i++) {
    Run.act_nd[i]=Grp[Run.Reactor->Fission_source_region].Nuclide->nd;
    Run.R_abs[i]=Run.R_abs[i]*86400.*1.e-24*TheCase.fiss_rate/sum;  // convert from #/s to #/d
    Run.R_fiss[i]=Run.R_fiss[i]*86400.*1.e-24*TheCase.fiss_rate/sum;
    Grp[Run.Reactor->Fission_source_region].Nuclide=Grp[Run.Reactor->Fission_source_region].Nuclide->NextNucl;
  }
  Grp[Run.Reactor->Fission_source_region].Nuclide->nd=Grp[Run.Reactor->Fission_source_region].Nuclide->nd+15.*(TheCase.fiss_rate*Run.Reactor->volratio)*2.*86400.*tstep*1.e-24;  // increment lumped fp concentration
  while (t_now <= tstep-dt) {  // step foward till we reach time=tstep
    step_forward(Run.act_nd,Run.R_abs,Run.R_fiss,dt);
    t_now=t_now+dt;
  }
  step_forward(Run.act_nd,Run.R_abs,Run.R_fiss,tstep-t_now);

  Grp[Run.Reactor->Fission_source_region].Nuclide=Grp[Run.Reactor->Fission_source_region].NuclFront;
  for(i=0; i<NACT; i++) {
    Run.R_abs[i]=Run.R_abs[i]/86400./1.e-24/TheCase.fiss_rate*sum;  // convert from #/s to #/d
    Run.R_fiss[i]=Run.R_fiss[i]/86400./1.e-24/TheCase.fiss_rate*sum;
    Grp[Run.Reactor->Fission_source_region].Nuclide=Grp[Run.Reactor->Fission_source_region].Nuclide->NextNucl;
  }

  Grp[Run.Reactor->Fission_source_region].Nuclide=Grp[Run.Reactor->Fission_source_region].NuclFront;
  for(i=0; i<NACT; i++) {
    //act_sum += Run.act_nd[i];
    Grp[Run.Reactor->Fission_source_region].Nuclide->nd=Run.act_nd[i];
    Grp[Run.Reactor->Fission_source_region].Nuclide=Grp[Run.Reactor->Fission_source_region].Nuclide->NextNucl;
  }
  //printf("ACT %e tot %e \n",act_sum,(act_sum+Grp[Run.Reactor->Fission_source_region].Nuclide->nd/2.));
}

void update_ca_conc(int reg, int nits, float sum, float last_k, float *ndvalue) {

  // control system to alter ca concentration and bring the reactor to critical

  const float w1=0.75;  // weights for ca iteration.  should sum to 1.
  const float w2=0.25;
  //const float w3=0.000;
  float ca_nd_guess,ratio1;
  int i;

  Run.k=Run.totfn/Run.totabs;
  ca_nd_guess=(Run.k*w1+1./nits*sum*w2);
  ratio1=fabs((Run.k-last_k)/(last_k-1.)); // PI control
  /*  if(last_k > 0. && Run.R_neut/Run.Source_strength > 1) {  // differential control: seems too touchy to be helpful */
/*      if(ratio1>1.) { */
/*        ca_nd_guess=ca_nd_guess-w3*(ratio1-1.); */
/*      } */
/*      else ca_nd_guess=ca_nd_guess+w3*(1.-ratio1); */
/*    } */
/*    else if (last_k > 0. && Run.R_neut/Run.Source_strength < 1) { */
/*      if(ratio1>1.) { */
/*        ca_nd_guess=ca_nd_guess+w3*(ratio1-1.); */
/*      } */
/*      else ca_nd_guess=ca_nd_guess-w3*(1.-ratio1); */
/*    } */
  Grp[reg].Nuclide=Grp[reg].NuclFront;
  for(i=0; i<NACT; i++) {  // assign number density to each actinide based upon mass
    Grp[reg].Nuclide=Grp[reg].Nuclide->NextNucl;
  }
  for(i=0; i<3; i++) { // skip ahead thru lumped fp and Xe&Sm to control absorbers
    Grp[reg].Nuclide=Grp[reg].Nuclide->NextNucl;
  }
  Grp[1-reg].Nuclide=Grp[1-reg].NuclFront;
  while(Grp[reg].Nuclide->ca_nd_factor > 0.) {  // assign ca number densities
    Grp[reg].Nuclide->nd=ca_nd_guess*Grp[reg].Nuclide->nd;
    Grp[1-reg].Nuclide->nd=ca_nd_guess*Grp[reg].Nuclide->nd;
    *ndvalue=Grp[reg].Nuclide->nd;
    Grp[reg].Nuclide=Grp[reg].Nuclide->NextNucl;
    Grp[1-reg].Nuclide=Grp[1-reg].Nuclide->NextNucl;
  }
  printf("%f %d %f %f\n",ca_nd_guess,nits,sum,Grp[1-reg].Nuclide->PrevNucl->nd);
}

void set_ca_conc(int reg, float value) {

  // manually sets ca concentration as specified by value

  int i;

  Grp[reg].Nuclide=Grp[reg].NuclFront;
  for(i=0; i<NACT; i++) {  // assign number density to each actinide based upon mass
    Grp[reg].Nuclide=Grp[reg].Nuclide->NextNucl;
  }
  for(i=0; i<3; i++) { // skip ahead thru lumped fp and Xe&Sm to control absorbers
    Grp[reg].Nuclide=Grp[reg].Nuclide->NextNucl;
  }
  Grp[1-reg].Nuclide=Grp[1-reg].NuclFront;
  while(Grp[reg].Nuclide->ca_nd_factor > 0.) {  // assign ca number densities
    Grp[reg].Nuclide->nd=value*Grp[reg].Nuclide->nd;
    Grp[1-reg].Nuclide->nd=value*Grp[reg].Nuclide->nd;
    Grp[reg].Nuclide=Grp[reg].Nuclide->NextNucl;
    Grp[1-reg].Nuclide=Grp[1-reg].Nuclide->NextNucl;
  }
}

void group_thin() {

  // thins fine groups at 10 fine groups per coarse group: averages to get coarse-group fluxes, updates cross sections for coarse group calculations.

  int i,j,k,act_count=0,fgn;
  float f_flux,e_current;
  //FILE *f;

  for(i=0; i<NFAST/10; i++) {
    Run.phi[i][0]=0.;
    Run.phi[i][1]=0.;
    for(j=0; j<10; j++) {
      Run.phi[i][0]=Run.phi[i][0]+Run.phifine[i*10+j][0];
      Run.phi[i][1]=Run.phi[i][1]+Run.phifine[i*10+j][1];
    }
  }
  for(j=0; j<=1; j++) {
    Grp[j].Nuclide=Grp[j].NuclFront;
    while(Grp[j].Nuclide != NULL) {
      for(i=0; i<NFAST/10; i++) {
        Grp[j].Nuclide->sigma_s_c[i]=0.;
        Grp[j].Nuclide->nubar_c[i]=0.;
        Grp[j].Nuclide->mu_lab_c[i]=0.;
        Grp[j].Nuclide->sigma_g_c[i]=0.;
        Grp[j].Nuclide->sigma_f_c[i]=0.;
        Grp[j].Nuclide->sigma_i_c[i]=0.;
        Grp[j].Nuclide->fn_rate_c[i]=0.;
        for(k=0; k<10; k++) {
          fgn=i*10+k;
          if(Grp[j].Nuclide->a > 220) {  // actinides
            f_flux=Run.phifine[fgn][j]*Grp[j].Nuclide->f_stored[fgn];
            Grp[j].Nuclide->sigma_s_c[i]=Grp[j].Nuclide->sigma_s_c[i]+actinide_array[act_count].lh.scat[fgn]*f_flux;
            Grp[j].Nuclide->sigma_g_c[i]=Grp[j].Nuclide->sigma_g_c[i]+actinide_array[act_count].lh.absp[fgn]*f_flux;
            Grp[j].Nuclide->sigma_f_c[i]=Grp[j].Nuclide->sigma_f_c[i]+actinide_array[act_count].lh.fiss[fgn]*f_flux;
            Grp[j].Nuclide->sigma_i_c[i]=Grp[j].Nuclide->sigma_i_c[i]+actinide_array[act_count].lh.inxs[fgn]*f_flux;
            Grp[j].Nuclide->nubar_c[i]=Grp[j].Nuclide->nubar_c[i]+actinide_array[act_count].lh.nubr[fgn]*Run.phifine[fgn][j];
            Grp[j].Nuclide->mu_lab_c[i]=Grp[j].Nuclide->mu_lab_c[i]+actinide_array[act_count].lh.scos[fgn]*Run.phifine[fgn][j];
          }
          else { // nonactinides
            f_flux=Run.phifine[fgn][j]*Grp[j].Nuclide->f_stored[fgn];
            Grp[j].Nuclide->sigma_s_c[i]=Grp[j].Nuclide->sigma_s_c[i]+Grp[j].Nuclide->rawdata->lh.scat[fgn]*f_flux;
            Grp[j].Nuclide->sigma_g_c[i]=Grp[j].Nuclide->sigma_g_c[i]+Grp[j].Nuclide->rawdata->lh.absp[fgn]*f_flux;
            Grp[j].Nuclide->mu_lab_c[i]=Grp[j].Nuclide->mu_lab_c[i]+Grp[j].Nuclide->rawdata->lh.scos[fgn]*Run.phifine[fgn][j];
          }
        }
        Grp[j].Nuclide->sigma_s_c[i]=Grp[j].Nuclide->sigma_s_c[i]/Run.phi[i][j];
        Grp[j].Nuclide->sigma_g_c[i]=Grp[j].Nuclide->sigma_g_c[i]/Run.phi[i][j];
        Grp[j].Nuclide->sigma_f_c[i]=Grp[j].Nuclide->sigma_f_c[i]/Run.phi[i][j];
        Grp[j].Nuclide->sigma_i_c[i]=Grp[j].Nuclide->sigma_i_c[i]/Run.phi[i][j];
        Grp[j].Nuclide->nubar_c[i]=Grp[j].Nuclide->nubar_c[i]/Run.phi[i][j];
        Grp[j].Nuclide->mu_lab_c[i]=Grp[j].Nuclide->mu_lab_c[i]/Run.phi[i][j];
        Grp[j].Nuclide->xs_c[i]=Grp[j].Nuclide->sigma_s_c[i]+Grp[j].Nuclide->sigma_g_c[i]+Grp[j].Nuclide->sigma_f_c[i]+Grp[j].Nuclide->sigma_i_c[i];
      }
      if(Grp[j].Nuclide->a > 220) act_count++;
      Grp[j].Nuclide=Grp[j].Nuclide->NextNucl;
    }
  }
   Grp[0].Nuclide=Grp[0].NuclFront;
}

void get_flux() {

  // sweeps through the 70 coarse groups to derive 140 group fluxes

  int i;
  float B;  // the reactor buckling
  float e_current;  // current energy
  float vr_use;

  for(i=0; i<2; i++) {
    Grp[i].Nuclide=Grp[i].NuclFront;
    while(Grp[i].Nuclide!= NULL) {
      Grp[i].Nuclide->cum_abs=0.;
      Grp[i].Nuclide->cum_fiss=0.;
      Grp[i].Nuclide->cum_fn=0.;
      Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    }
  }
  InitializeNuclReg(*Run.insc,NFAST/10);  // clear structs for new run
  // begin downward sweep in energy to derive the flux: first using the fine-group structure
  for(Run.ncurrent=0; Run.ncurrent < NFAST/10; Run.ncurrent++) {
    e_current=get_e(Run.ncurrent,10.e6,DELTAL*DELTAFAST*10.);
    GetRawData(Run.ncurrent);
    get_sigma_tr();       // obtain transport cross sections for self shielding, transport probability, diffusion coeff calculations
    update_xs_c();          // derives correct macroscopic cross sections for each region
    get_selfscatter();     // we now have all we need to compute the fluxes in this group: self-shielded macroscopic cross sections (Sigma), inscattering source (from inscatter), fission source, probabilities (Pi), volume ratio (volratio), leakage term (db2), selfscattering xs (Sigma_ss).  Solve multigroup equations for the group fluxes.
    get_probs(&Grp[0].Pi,&Grp[1].Pi);  // derives transmission probabilities
    Run.Reactor->db2=get_diff_coeff(Run.phi[Run.ncurrent][0],Run.phi[Run.ncurrent][1]);
    B=get_buckling(e_current,Run.Reactor->db2,Run.Reactor->core_r_equiv)/Run.Reactor->core_r_equiv;  // find B*R from transcendantal eq. & divide through by R to get B
    Run.Reactor->db2=Run.Reactor->db2*B*B;
    Grp[0].Sigma_tr=Grp[0].Sigma-Grp[0].Sigma_ss;
    Grp[1].Sigma_tr=Grp[1].Sigma-Grp[1].Sigma_ss;
    get_probs(&Grp[0].Pi,&Grp[1].Pi);  // derives transmission probabilities
    deduct_selfscatter();
    augment_inscatter();  // update inscatter array to reflect this group's downscattering contributions
    augment_abs_fiss();
    for(i=0; i<2; i++) {
      Grp[i].Nuclide=Grp[i].NuclFront;
      if(i==0) {
          vr_use=Run.Reactor->volratio;
      } else {
          vr_use=1.;
      }
      while(Grp[i].Nuclide!= NULL) {
        Grp[i].Nuclide->cum_abs+=(Grp[i].Nuclide->sigma_f+Grp[i].Nuclide->sigma_g)*Grp[i].Nuclide->nd*Run.phi[Run.ncurrent][i]*vr_use;
        Grp[i].Nuclide->cum_fiss+=Grp[i].Nuclide->sigma_f*Grp[i].Nuclide->nd*Run.phi[Run.ncurrent][i]*vr_use;
        Grp[i].Nuclide->cum_fn+=Grp[i].Nuclide->sigma_f*Grp[i].Nuclide->nubar*Grp[i].Nuclide->nd*Run.phi[Run.ncurrent][i]*vr_use;
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
      }
    }
  }
  Run.fast_escape=Run.escape;
  Run.fast_fn=Run.totfn;
  //printf("Fast: abs: %e fissneut: %e escape:%e fiss:%e fuelabs: %e\n",Run.totabs,Run.totfn,Run.escape,Run.totfiss,Run.absfuelfast);
}

void augment_abs_fiss() {

  // augments the tally of absorptions, fissions, neutrons produced

  int actcnt={0},i;

  Grp[Run.Reactor->Fission_source_region].Nuclide=Grp[Run.Reactor->Fission_source_region].NuclFront;
  if(Run.ncurrent == 0) {
    Run.totabs=0.;
    Run.escape=0.;
    Run.totfn=0.;
    Run.ca_abs=0.;
    Run.totfiss=0.;
    Run.absfuelth=0.;
    Run.absfuelfast=0.;
    Run.absorbed_th=0.;
    for(i=0; i<NACT; i++) {
      Run.R_abs[i]=0.;
      Run.R_fiss[i]=0.;
      Run.one_grp_xs[i][0]=0.;
      Run.one_grp_xs[i][1]=0.;
    }
  }
  while(actcnt < NACT) {
    Run.R_abs[actcnt]=Run.R_abs[actcnt]+Grp[Run.Reactor->Fission_source_region].Nuclide->sigma_g_c[Run.ncurrent]*Run.phi[Run.ncurrent][Run.Reactor->Fission_source_region];
    Run.R_fiss[actcnt]=Run.R_fiss[actcnt]+Grp[Run.Reactor->Fission_source_region].Nuclide->sigma_f_c[Run.ncurrent]*Run.phi[Run.ncurrent][Run.Reactor->Fission_source_region];
    Run.absfuelfast=Run.absfuelfast+(Grp[0].Nuclide->sigma_g_c[Run.ncurrent]+Grp[0].Nuclide->sigma_f_c[Run.ncurrent])*Run.phi[Run.ncurrent][0]*Grp[0].Nuclide->nd*Run.Reactor->volratio;
    Run.one_grp_xs[actcnt][0] += Grp[Run.Reactor->Fission_source_region].Nuclide->sigma_g_c[Run.ncurrent]*Run.phi[Run.ncurrent][Run.Reactor->Fission_source_region];
    Run.one_grp_xs[actcnt][1] += Grp[Run.Reactor->Fission_source_region].Nuclide->sigma_f_c[Run.ncurrent]*Run.phi[Run.ncurrent][Run.Reactor->Fission_source_region];
    Grp[Run.Reactor->Fission_source_region].Nuclide=Grp[Run.Reactor->Fission_source_region].Nuclide->NextNucl;
    actcnt++;
  }
   Run.totabs=Run.totabs+Run.Reactor->db2*Run.phi[Run.ncurrent][1]+Run.Reactor->volratio*Run.Reactor->db2*Run.phi[Run.ncurrent][0];
   Run.escape=Run.escape+Run.Reactor->db2*Run.phi[Run.ncurrent][1]+Run.Reactor->volratio*Run.Reactor->db2*Run.phi[Run.ncurrent][0];
  for(i=0; i<=1; i++) {
    Grp[i].Nuclide=Grp[i].NuclFront;
    while(Grp[i].Nuclide != NULL) {
      if (i==0) {
        Run.totabs=Run.totabs+(Grp[i].Nuclide->sigma_g_c[Run.ncurrent]+Grp[i].Nuclide->sigma_f_c[Run.ncurrent])*Run.phi[Run.ncurrent][i]*Grp[i].Nuclide->nd*Run.Reactor->volratio;
        Run.totfn=Run.totfn+(Grp[i].Nuclide->sigma_f_c[Run.ncurrent])*Run.phi[Run.ncurrent][i]*Grp[i].Nuclide->nd*Run.Reactor->volratio*Grp[i].Nuclide->nubar_c[Run.ncurrent];
        Run.totfiss=Run.totfiss+(Grp[i].Nuclide->sigma_f_c[Run.ncurrent])*Run.phi[Run.ncurrent][i]*Grp[i].Nuclide->nd*Run.Reactor->volratio;
        if (Grp[i].Nuclide->ca_nd_factor > 0.) Run.ca_abs=Run.ca_abs+(Grp[i].Nuclide->sigma_g_c[Run.ncurrent])*Run.phi[Run.ncurrent][i]*Grp[i].Nuclide->nd*Run.Reactor->volratio;
      }
      else {
        Run.totabs=Run.totabs+(Grp[i].Nuclide->sigma_g_c[Run.ncurrent]*Run.phi[Run.ncurrent][i]*Grp[i].Nuclide->nd);
        if (Grp[i].Nuclide->ca_nd_factor > 0.) Run.ca_abs=Run.ca_abs+(Grp[i].Nuclide->sigma_g_c[Run.ncurrent])*Run.phi[Run.ncurrent][i]*Grp[i].Nuclide->nd;
      }
      Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    }
  }
}

void get_next_inscatter(int i) {

  // just for testing

  int j,k;

  Run.insc[i][0]=0.;
  Run.insc[i][1]=0.;
  for(j=0; j<=1; j++) {
    Grp[j].Nuclide=Grp[j].NuclFront;
    while(Grp[j].Nuclide != NULL) {
      k=1;
      while(k<=Grp[j].Nuclide->ndown && i-k >=0) {
        Run.insc[i][j]=Run.insc[i][j]+(Grp[j].Nuclide->downscatter[k][0])*Grp[j].Nuclide->nd*Run.phi[i-k][j]*Grp[j].Nuclide->sigma_s_c[i-k];
        k++;
      }
      Grp[j].Nuclide=Grp[j].Nuclide->NextNucl;
    }
  }
}

void get_fission_spec_c() {

  // another annoying utility; coarsens fission spectrum
  int i,j;

  for(i=0; i<NFAST/10; i++) {
    fiss_spec_c[i]=0.;
    for (j=0; j<10; j++) {
      fiss_spec_c[i]=fiss_spec_c[i]+fiss_spec[i*10+j];
    }
  }
}

void get_inel_kernel_c() {

  // coarsens inelastic scattering kernel

  int i,j,k,l;

  for(i=0; i<NHI/10; i++) {
    for(j=0; j<NHI/10; j++) {
      globalinel_c[i][j]=0.;
      for(k=0; k<10; k++) {
        for(l=0; l<10; l++) {
          globalinel_c[i][j]+=globalinel[i*10+k][j*10+l]/10.;
        }
      }
    }
  }
}

void get_th_flux() {

  // Solves 80x80 system of coupled equations Ax=b where A=th_matrix, x=th_flux, b=insc_th to obtain thermal group fluxes. First assembles the th_matrix

  int i,j,l,actcnt;
  float e_current, B, vr_use;  // current energy, buckling, volratio dummy
  float dummy_thermalized=0;
  int indx[NTH*2]; // for ludcmp

  Run.thermalized=0.;
  for(i=0; i<NTH*2; i++) {
    for(j=0; j<NTH*2; j++) {
      Run.th_matrix[i][j]=0.;
    }
  }
  for(i=0; i<NTH; i++) {
    Run.thermalized=Run.thermalized+Run.insc_th[2*i]*Run.Reactor->volratio+Run.insc_th[2*i+1];
    e_current=get_e(i,1.,DELTAL*DELTAFAST*10.);
    GetRawDataTh(i);
    get_sigma_tr();       // obtain transport cross sections for self shielding, transport probability, diffusion coeff calculations
    update_xs_c_th();          // derives correct macroscopic cross sections for each region
    Run.Reactor->db2=get_diff_coeff(1.,1.);
    B=get_buckling(e_current,Run.Reactor->db2,Run.Reactor->core_r_equiv)/Run.Reactor->core_r_equiv;  // find B*R from transcendantal eq. & divide through by R to get B
    Run.Reactor->db2=Run.Reactor->db2*B*B;
    Run.db2s[i]=Run.Reactor->db2;
    get_selfscatter_th(i);
    Grp[0].Sigma_tr=Grp[0].Sigma-Grp[0].Sigma_ss;
    Grp[1].Sigma_tr=Grp[1].Sigma-Grp[1].Sigma_ss;
    deduct_selfscatter();
    get_probs(&Grp[0].Pi,&Grp[1].Pi);  // derives transmission probabilities
    for(j=0; j<NTH; j++) {
      if(j==i) {  // selfscattering diagonal entry: region 0
        Run.th_matrix[2*i][2*i]=(Grp[0].Sigma+Run.Reactor->db2)*Run.Reactor->volratio;//-(1.-Grp[0].Pi)*Grp[0].Sigma_sk[i];
        Run.th_matrix[2*i][2*i+1]=0;//-Grp[1].Pi/Run.Reactor->volratio*Grp[1].Sigma_sk[j];
      }
      else {  // other entries row 2*i
        Run.th_matrix[2*i][2*j]=-(1.-Grp[0].Pi)*Grp[0].Sigma_sk[j]*Run.Reactor->volratio;
        Run.th_matrix[2*i][2*j+1]=-Grp[1].Pi*Grp[1].Sigma_sk[j];
      }
      if(j==i) {  // selfscattering diagonal entry: region 1
        Run.th_matrix[2*i+1][2*i+1]=(Grp[1].Sigma+Run.Reactor->db2);//-(1.-Grp[1].Pi)*Grp[1].Sigma_sk[i];
        Run.th_matrix[2*i+1][2*i]=0;//-Grp[0].Pi*Run.Reactor->volratio*Grp[0].Sigma_sk[j];
      }
      else {  // other entries row 2*i+1
        Run.th_matrix[2*i+1][2*j+1]=-(1.-Grp[1].Pi)*Grp[1].Sigma_sk[j];
        Run.th_matrix[2*i+1][2*j]=-Grp[0].Pi*Grp[0].Sigma_sk[j]*Run.Reactor->volratio;
      }
      Run.inscatter_by_group[2*i][2*j]+=(1.-Grp[0].Pi)*Grp[0].Sigma_sk[j]*Run.Reactor->volratio;
      Run.inscatter_by_group[2*i][2*j+1]+=Grp[1].Pi*Grp[1].Sigma_sk[j];
      Run.inscatter_by_group[2*i+1][2*j]+=Grp[0].Pi*Grp[0].Sigma_sk[j]*Run.Reactor->volratio;
      Run.inscatter_by_group[2*i+1][2*j+1]+=(1.-Grp[1].Pi)*Grp[1].Sigma_sk[j];
    }
    Run.abs_by_group[NFAST/10+i][0]=(Grp[0].Sigma-Grp[0].Sigma_s+Run.Reactor->db2)*Run.Reactor->volratio;
    Run.abs_by_group[NFAST/10+i][1]=(Grp[1].Sigma-Grp[1].Sigma_s+Run.Reactor->db2);
    Run.outscatter_by_group[NFAST/10+i][0]=Grp[0].Sigma_s*Run.Reactor->volratio;
    Run.outscatter_by_group[NFAST/10+i][1]=Grp[1].Sigma_s;
    Run.phi_th[2*i]=(1.-Grp[0].Pi)*Run.insc_th[2*i]*Run.Reactor->volratio+Grp[1].Pi*Run.insc_th[2*i+1];
    Run.phi_th[2*i+1]=(1.-Grp[1].Pi)*Run.insc_th[2*i+1]+Grp[0].Pi*Run.insc_th[2*i]*Run.Reactor->volratio;
    Run.source_by_group[NFAST/10+i][0]=Run.phi_th[2*i];
    Run.source_by_group[NFAST/10+i][1]=Run.phi_th[2*i+1];
    dummy_thermalized+=Run.phi_th[2*i]+Run.phi_th[2*i+1];
  }
  // we've populated the Run.th_matrix & Run.phi_th (source term) structures.  now the lu decomposition.
  ludcmp(*Run.th_matrix,NTH*2,indx,&B);
  lubksb(*Run.th_matrix,NTH*2,indx,Run.phi_th);
  for(i=0;i<NTH;i++) {
      actcnt=0;
      if(Run.phi_th[2*i] < 0.) {
          Run.phi_th[2*i]=1.;
      }
      if(Run.phi_th[2*i+1] < 0.) {
          Run.phi_th[2*i+1]=1.;
      }
      for(j=0; j<2; j++) {
          Grp[j].Nuclide=Grp[j].NuclFront;
          if(j==0) {
            vr_use=Run.Reactor->volratio;
          } else {
            vr_use=1.;
          }
          while(Grp[j].Nuclide != NULL) {
            if (j==Run.Reactor->Fission_source_region) {
              if(actcnt < NACT) {
                Grp[j].Nuclide->cum_abs+=(actinide_array[actcnt].th.absp[i]+actinide_array[actcnt].th.fiss[i])*Run.phi_th[2*i+j]*Grp[j].Nuclide->nd*vr_use;
                Grp[j].Nuclide->cum_fiss+=actinide_array[actcnt].th.fiss[i]*Run.phi_th[2*i+j]*Grp[j].Nuclide->nd*vr_use;
                Grp[j].Nuclide->cum_fn+=actinide_array[actcnt].th.fiss[i]*actinide_array[actcnt].th.nubr[i]*Run.phi_th[2*i+j]*Grp[j].Nuclide->nd*vr_use;
                actcnt++;
              }
              else {
                Grp[j].Nuclide->cum_abs+=(Grp[j].Nuclide->rawdata->th.absp[i])*Run.phi_th[2*i+j]*Grp[j].Nuclide->nd*vr_use;
              }
            }
            else {
                Grp[j].Nuclide->cum_abs+=(Grp[j].Nuclide->rawdata->th.absp[i])*Run.phi_th[2*i+j]*Grp[j].Nuclide->nd*vr_use;
            }
            Grp[j].Nuclide=Grp[j].Nuclide->NextNucl;
          }
      }
  }
  augment_abs_fiss_th();
  // printf("Total: fission neutrs:%e abs:%e escape:%e total fissions:%e Thermalized:%e OtherThermalized: %e Abs Total Th: %e Abs Fuel Th:%e Source Strength: %e\n",Run.totfn,Run.totabs,Run.escape,Run.totfiss,Run.thermalized,dummy_thermalized,Run.absorbed_th,Run.absfuelth,Run.Source_strength);
}

void InitializeThReg() {

  // wipes stuff used by thermal flux model

  int i;

  for(i=0; i<=NTH*2; i++) {

    Run.insc_th[i]=0.;
  }
}

void augment_abs_fiss_th() {

  // augments the tally of absorptions, fissions, neutrons produced: thermal range

  int actcnt={0},i,j;

  Grp[Run.Reactor->Fission_source_region].Nuclide=Grp[Run.Reactor->Fission_source_region].NuclFront;
  while(actcnt < NACT) {
    for(i=0; i<NTH; i++) {
      Run.absfuelth=Run.absfuelth+(actinide_array[actcnt].th.absp[i]+actinide_array[actcnt].th.fiss[i])*Run.phi_th[2*i]*Grp[0].Nuclide->nd*Run.Reactor->volratio;
      Run.R_abs[actcnt]=Run.R_abs[actcnt]+actinide_array[actcnt].th.absp[i]*Run.phi_th[2*i+Run.Reactor->Fission_source_region];
      Run.R_fiss[actcnt]=Run.R_fiss[actcnt]+actinide_array[actcnt].th.fiss[i]*Run.phi_th[2*i+Run.Reactor->Fission_source_region];
    Run.one_grp_xs[actcnt][0] += actinide_array[actcnt].th.absp[i]*Run.phi_th[2*i+Run.Reactor->Fission_source_region];
    Run.one_grp_xs[actcnt][1] += actinide_array[actcnt].th.fiss[i]*Run.phi_th[2*i+Run.Reactor->Fission_source_region];
    }
    Grp[Run.Reactor->Fission_source_region].Nuclide=Grp[Run.Reactor->Fission_source_region].Nuclide->NextNucl;
    actcnt++;
  }
  actcnt=0.;
  for(j=0; j<NTH; j++) {
    Run.totabs=Run.totabs+Run.db2s[j]*Run.phi_th[2*j+1]+Run.Reactor->volratio*Run.db2s[j]*Run.phi_th[2*j];
    Run.escape=Run.escape+Run.db2s[j]*Run.phi_th[2*j+1]+Run.Reactor->volratio*Run.db2s[j]*Run.phi_th[2*j];
    Run.absorbed_th=Run.absorbed_th+Run.db2s[j]*Run.phi_th[2*j+1]+Run.Reactor->volratio*Run.db2s[j]*Run.phi_th[2*j];
    for(i=0; i<=1; i++) {
      Grp[i].Nuclide=Grp[i].NuclFront;
      actcnt=0.;
      Grp[Run.Reactor->Fission_source_region].Nuclide=Grp[Run.Reactor->Fission_source_region].NuclFront;
      while(Grp[i].Nuclide != NULL) {
        if (i==0) {
          if(actcnt < NACT) {
            Run.totabs=Run.totabs+(actinide_array[actcnt].th.absp[j]+actinide_array[actcnt].th.fiss[j])*Run.phi_th[2*j]*Grp[i].Nuclide->nd*Run.Reactor->volratio;
            Run.totfn=Run.totfn+(actinide_array[actcnt].th.fiss[j])*Run.phi_th[2*j]*Grp[i].Nuclide->nd*Run.Reactor->volratio*actinide_array[actcnt].th.nubr[j];
            Run.totfiss=Run.totfiss+(actinide_array[actcnt].th.fiss[j])*Run.phi_th[2*j]*Grp[i].Nuclide->nd*Run.Reactor->volratio;
            Run.absorbed_th=Run.absorbed_th+(actinide_array[actcnt].th.absp[j]+actinide_array[actcnt].th.fiss[j])*Run.phi_th[2*j]*Grp[i].Nuclide->nd*Run.Reactor->volratio;
            actcnt++;
          }
          else {
            Run.totabs=Run.totabs+(Grp[i].Nuclide->rawdata->th.absp[j])*Run.phi_th[2*j]*Grp[i].Nuclide->nd*Run.Reactor->volratio;
            Run.absorbed_th=Run.absorbed_th+(Grp[i].Nuclide->rawdata->th.absp[j])*Run.phi_th[2*j]*Grp[i].Nuclide->nd*Run.Reactor->volratio;
            if (Grp[i].Nuclide->ca_nd_factor > 0.) Run.ca_abs=Run.ca_abs+Grp[i].Nuclide->rawdata->th.absp[j]*Run.phi_th[2*j]*Grp[i].Nuclide->nd*Run.Reactor->volratio;
          }
        }
        else {
          Run.totabs=Run.totabs+(Grp[i].Nuclide->rawdata->th.absp[j]*Run.phi_th[2*j+1]*Grp[i].Nuclide->nd);
          Run.absorbed_th=Run.absorbed_th+(Grp[i].Nuclide->rawdata->th.absp[j]*Run.phi_th[2*j+1]*Grp[i].Nuclide->nd);
          if (Grp[i].Nuclide->ca_nd_factor > 0.) Run.ca_abs=Run.ca_abs+Grp[i].Nuclide->rawdata->th.absp[j]*Run.phi_th[2*j+1]*Grp[i].Nuclide->nd;
        }
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
      }
    }
  }
}

void deduct_selfscatter() {

  int i;

  for(i=0; i<2; i++) {
    Grp[i].Sigma=Grp[i].Sigma-Grp[i].Sigma_ss;
    Grp[i].Sigma_s=Grp[i].Sigma_s-Grp[i].Sigma_ss;
  }
}

void get_fine_flux() {

  // performs the initial sweep to solve for the fine-group fluxes

  float B;  // the reactor buckling
  float e_current;  // current energy
  float stored_pi[2];

  // begin downward sweep in energy to derive the flux: first using the fine-group structure
  InitializeNuclReg(*Run.inscfine,NFAST);  // update structs for this run, wiping things that may be nonzero
  for(Run.ncurrent=0; Run.ncurrent < NFAST; Run.ncurrent++) {
    e_current=get_e(Run.ncurrent,10.e6,DELTAL*DELTAFAST);
    GetRawDataFine(Run.ncurrent);
    get_sigma_tr();       // obtain transport cross sections for self shielding, transport probability, diffusion coeff calculations
    selfshield();         // obtain self shielding factors
    update_xs();          // uses self shielding factors to update microscopic cross sections; derives correct macroscopic cross sections for each region
    get_selfscatterfine();           // we now have all we need to compute the fluxes in this group: self-shielded macroscopic cross sections (Sigma), inscattering source (from inscatterfine), fission source, probabilities (Pi), volume ratio (volratio), leakage term (db2), selfscattering xs (Sigma_ss).  Solve multigroup equations for the group fluxes.
    get_probs(&Grp[0].Pi,&Grp[1].Pi);
    stored_pi[0]=Grp[0].Pi;
    stored_pi[1]=Grp[1].Pi;
    Run.Reactor->db2=get_diff_coeff(1.,1.);
    B=get_buckling(e_current,Run.Reactor->db2,Run.Reactor->core_r_equiv)/Run.Reactor->core_r_equiv;  // find B*R from transcendantal eq. & divide through by R to get B
    Run.Reactor->db2=Run.Reactor->db2*B*B;
    Grp[0].Sigma_tr=Grp[0].Sigma-Grp[0].Sigma_ss;
    Grp[1].Sigma_tr=Grp[1].Sigma-Grp[1].Sigma_ss;
    deduct_selfscatter();
    get_probs(&Grp[0].Pi,&Grp[1].Pi);
    solve_multigroup(&Run.phifine[Run.ncurrent][0],&Run.phifine[Run.ncurrent][1],fiss_spec[Run.ncurrent]*Run.Source_strength/Run.Reactor->volratio,Run.Reactor->Fission_source_region,Run.inscfine[Run.ncurrent][0],Run.inscfine[Run.ncurrent][1]);
    augment_inscatterfine();  // update inscatter array to reflect this group's downscattering contributions
  }
}

float get_e(int nc, float e0, float du) {

  // gets the energy of the current lethargy group nc groups below top group of energy e0; bin width du

  return(e0*exp(-du*nc));
}

float get_diff_coeff(float phi0, float phi1) {

  // obtains the diffusion coefficient D=1/(3*Sigma_tr); flux weighted.  For initial fine group sweep, where fluxes are not a priori known, flux weighting is off
  return((Run.Reactor->volratio*phi0+phi1)/(3.*(Run.Reactor->volratio*phi0*Grp[0].Sigma_tr+phi1*Grp[1].Sigma_tr)));
}

void get_selfscatter_th(int group) {

  // Derives macroscopic total selfscattering cross section for group for both regions: used in solution of multigroup equations

  int i;
  float sstot;

  for(i=0; i<=1; i++) {
    sstot=0.;
    Grp[i].Nuclide=Grp[i].NuclFront;
    while (Grp[i].Nuclide != NULL) {  // go through each nuclide; add its contribution
      sstot=sstot+Grp[i].Nuclide->nd*Grp[i].Nuclide->sc_krnl[group];
      Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    }
    Grp[i].Sigma_ss=sstot;
  }
}

void get_selfscatter() {

  // Derives macroscopic total selfscattering cross section for both regions: used in solution of multigroup equations

  int i;
  float tend,sstot;

  for(i=0; i<=1; i++) {
    sstot=0.;
    Grp[i].Nuclide=Grp[i].NuclFront;
    while (Grp[i].Nuclide != NULL) {  // go through each nuclide; add its contribution
      if (Grp[i].Nuclide->ndown == -1) {  // hydrogen-1
        tend=pow(10.,DELTAFAST*10.); // appropriate for coarse group structure
        sstot=sstot+Grp[i].Nuclide->nd*Grp[i].Nuclide->sigma_s/(DELTAL*DELTAFAST*10.)*(1./tend+DELTAFAST*10.*DELTAL-1.);  // selfscattering
      }
      else {                              // everything else
        sstot=sstot+Grp[i].Nuclide->nd*Grp[i].Nuclide->sigma_s*(Grp[i].Nuclide->downscatter[0][0]+Grp[i].Nuclide->downscatter[0][1]*Grp[i].Nuclide->mu_cm);
        if(Grp[i].Nuclide->a > 200) sstot+=Grp[i].Nuclide->nd*Grp[i].Nuclide->sigma_i*Grp[i].Nuclide->inel_ss;  // inelastic selfscatter
      }
      Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    }
    Grp[i].Sigma_ss=sstot;
  }
}

void get_selfscatterfine() {

  // Derives macroscopic total selfscattering cross section for both regions: used in solution of multigroup equations

  int i;
  float tend,sstot;

  for(i=0; i<=1; i++) {
    sstot=0.;
    Grp[i].Nuclide=Grp[i].NuclFront;
    while (Grp[i].Nuclide != NULL) {  // go through each nuclide; add its contribution
      if (Grp[i].Nuclide->ndownfine == -1) {  // hydrogen-1
        tend=pow(10.,DELTAFAST); // appropriate for fine group structure
        sstot=sstot+Grp[i].Nuclide->nd*Grp[i].Nuclide->sigma_s/(DELTAL*DELTAFAST)*(1./tend+DELTAFAST*DELTAL-1.);  // selfscattering
      }
      else {                              // everything else
        sstot=sstot+Grp[i].Nuclide->nd*Grp[i].Nuclide->sigma_s*(Grp[i].Nuclide->downscatterfine[0][0]+Grp[i].Nuclide->downscatterfine[0][1]*Grp[i].Nuclide->mu_cm);
        if(Grp[i].Nuclide->a > 200) sstot+=Grp[i].Nuclide->nd*Grp[i].Nuclide->sigma_i*Grp[i].Nuclide->inel_ss;  // inelastic selfscatter
      }
      Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    }
    Grp[i].Sigma_ss=sstot;
  }
}

void update_xs() {

  // updates cross sections taking into account f-factor values: multiplies xs (total), sigma_g, sigma_f, sigma_s, sigma_i by f_calc: obtains correct values for macroscopic scattering, absorption, fission, total xs.

  int i;
  float ST={0.},SF={0.},SA={0.},SS={0.}, SI={0.};

  for(i=0; i<=1; i++) {
    Grp[i].Nuclide=Grp[i].NuclFront;
    while (Grp[i].Nuclide != NULL) {
      Grp[i].Nuclide->xs=Grp[i].Nuclide->xs*Grp[i].Nuclide->f_calc;
      ST=ST+Grp[i].Nuclide->xs*Grp[i].Nuclide->nd;
      Grp[i].Nuclide->sigma_s=Grp[i].Nuclide->sigma_s*Grp[i].Nuclide->f_calc;
      SS=SS+Grp[i].Nuclide->sigma_s*Grp[i].Nuclide->nd;
      Grp[i].Nuclide->sigma_f=Grp[i].Nuclide->sigma_f*Grp[i].Nuclide->f_calc;
      SF=SF+Grp[i].Nuclide->sigma_f*Grp[i].Nuclide->nd;
      Grp[i].Nuclide->sigma_i=Grp[i].Nuclide->sigma_i*Grp[i].Nuclide->f_calc;
      SI=SI+Grp[i].Nuclide->sigma_i*Grp[i].Nuclide->nd;
      Grp[i].Nuclide->sigma_g=Grp[i].Nuclide->sigma_g*Grp[i].Nuclide->f_calc;
      SA=SA+Grp[i].Nuclide->sigma_g*Grp[i].Nuclide->nd;
      Grp[i].Nuclide->sigma_tr=Grp[i].Nuclide->sigma_tr*Grp[i].Nuclide->f_calc;  // Sigma_tr was already correctly computed (w/ self shielding) prior to transmission probability determination
      Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    }
    Grp[i].Sigma=ST; ST=0.;
    Grp[i].Sigma_s=SS; SS=0.;
    Grp[i].Sigma_g=SA; SA=0.;
    Grp[i].Sigma_f=SF; SF=0.;
    Grp[i].Sigma_i=SI; SI=0.;
  }
}

void update_xs_c() {

  // updates cross sections: multiplies xs (total), sigma_g, sigma_f,sigma_s, sigma_i by nd: obtains correct values for macroscopic scattering, absorption, fission, total, transport xs.

  int i,actcnt={0};
  float ST={0.},SF={0.},SA={0.},SS={0.},STR={0.}, SI={0.};

  for(i=0; i<=1; i++) {
    Grp[i].Nuclide=Grp[i].NuclFront;
    while (Grp[i].Nuclide != NULL) {
      ST=ST+Grp[i].Nuclide->xs*Grp[i].Nuclide->nd;
      SS=SS+Grp[i].Nuclide->sigma_s*Grp[i].Nuclide->nd;
      SF=SF+Grp[i].Nuclide->sigma_f*Grp[i].Nuclide->nd;
      SA=SA+Grp[i].Nuclide->sigma_g*Grp[i].Nuclide->nd;
      SI=SI+Grp[i].Nuclide->sigma_i*Grp[i].Nuclide->nd;
      STR=STR+Grp[i].Nuclide->sigma_tr*Grp[i].Nuclide->nd;
      //if(actcnt==I_U_238) printf("%d %f %f %f\n",Run.ncurrent,Grp[i].Nuclide->sigma_g,Grp[i].Nuclide->nd,ST);
      Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
      actcnt++;
    }
    Grp[i].Sigma=ST; ST=0.;
    Grp[i].Sigma_s=SS; SS=0.;
    Grp[i].Sigma_g=SA; SA=0.;
    Grp[i].Sigma_f=SF; SF=0.;
    Grp[i].Sigma_tr=STR; STR=0.;
    Grp[i].Sigma_i=SI; SI=0.;
  }
  //printf("%d %f %f %f\n",Run.ncurrent,Grp[1].Sigma,Grp[1].Sigma_s,Grp[1].Sigma_g);
}

void update_xs_c_th() {

  // updates cross sections: multiplies xs (total), sigma_g, sigma_f, sigma_s by nd: obtains correct values for macroscopic scattering, absorption, fission, total, transport xs, scattering kernel, thermal energies.

  int i,j;
  float ST={0.},SF={0.},SA={0.},SS={0.},STR={0.},SK[NTH]={0.};

  for(i=0; i<=1; i++) {
    Grp[i].Nuclide=Grp[i].NuclFront;
    while (Grp[i].Nuclide != NULL) {
      ST=ST+Grp[i].Nuclide->xs*Grp[i].Nuclide->nd;
      SS=SS+Grp[i].Nuclide->sigma_s*Grp[i].Nuclide->nd;
      SF=SF+Grp[i].Nuclide->sigma_f*Grp[i].Nuclide->nd;
      SA=SA+Grp[i].Nuclide->sigma_g*Grp[i].Nuclide->nd;
      STR=STR+Grp[i].Nuclide->sigma_tr*Grp[i].Nuclide->nd;
      for(j=0; j<NTH; j++) {
        SK[j]=SK[j]+Grp[i].Nuclide->sc_krnl[j]*Grp[i].Nuclide->nd;
      }
      Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    }
    Grp[i].Sigma=ST; ST=0.;
    Grp[i].Sigma_s=SS; SS=0.;
    Grp[i].Sigma_g=SA; SA=0.;
    Grp[i].Sigma_f=SF; SF=0.;
    Grp[i].Sigma_tr=STR; STR=0.;
    for(j=0; j<NTH; j++) {
      Grp[i].Sigma_sk[j]=SK[j];
      SK[j]=0.;
    }
  }
}

void get_sigma_tr() {

  // finds the transport cross section from the total cross section and the lab-frame average scattering cosine; also derives the cm frame average scattering cosine

  int i;

  for(i=0; i<=1; i++) {
    Grp[i].Nuclide=Grp[i].NuclFront;
    while (Grp[i].Nuclide != NULL) {
      Grp[i].Nuclide->mu_cm=convert_mu(Grp[i].Nuclide->mu_lab,Grp[i].Nuclide->a);
      Grp[i].Nuclide->sigma_tr=Grp[i].Nuclide->xs-Grp[i].Nuclide->mu_lab*Grp[i].Nuclide->sigma_s;
      Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    }
  }
}

float get_buckling(float e_current, float D_core, float R_equiv) {

  // calculates the buckling assuming a spherical reactor of radius R_equiv surrounded by an infinite graphite reflector (graphite is treated w/ 1/v absorption and constant scattering x-s)

  float Sigma_aREFL, LREFL;
  int j;
  float dx,f,fmid,xmid,rtb,x1,x2;

  x1=EPS;  // roots of this equation lie between 0 and pi
  x2=M_PI-EPS;
  Sigma_aREFL= pow(0.0253/e_current,0.5)*SAREFL;
  LREFL=pow(DREFL/Sigma_aREFL,0.5);
  f=buckle_func(x1,LREFL,D_core,R_equiv);
  fmid=buckle_func(x2,LREFL,D_core,R_equiv);
  if (f*fmid >= 0.0) standard_error("BR must be bracketed in get_buckling");
  rtb = f < 0.0 ? (dx=x2-x1,x1) : (dx=x1-x2,x2);
  for (j=1;j<=40;j++) {
    xmid=rtb+(dx *= 0.5);
    fmid=buckle_func(xmid,LREFL,D_core,R_equiv);
    if (fmid <= 0.0) rtb=xmid;
    if (fabs(dx) < .01 || fmid == 0.0) return rtb;
  }
  standard_error("Too many bisections in get_buckling");
  return 0.0;
  //return(sqrt((2.405/Run.Reactor->core_r)*(2.405/Run.Reactor->core_r)+(3.14/Run.Reactor->core_h)*(3.14/Run.Reactor->core_h)));
}


float buckle_func(float x, float lr, float dc, float re) {

  // the buckling function to be zeroed

  return(1.-x*cos(x)/sin(x)-DREFL/dc*(re/lr+1.));

}

void solve_multigroup(float *phi0, float *phi1, float fs, int fsreg, float in0, float in1) {

  // Solves the coupled multigroup equations in regions 0 and 1 for the fluxes

  float denom0, denom1;

  denom1=Grp[1].Sigma+Run.Reactor->db2;//-(1.-Grp[1].Pi)*Grp[1].Sigma_ss;
  denom0=Grp[0].Sigma+Run.Reactor->db2;//-(1.-Grp[0].Pi)*Grp[0].Sigma_ss-Grp[1].Pi*Grp[1].Sigma_ss*(Grp[0].Pi*Grp[0].Sigma_ss/denom1);
  *phi0=((1.-Grp[0].Pi)*(in0+fs*(1-fsreg))+1./Run.Reactor->volratio*Grp[1].Pi*(in1+fs*fsreg+0*Grp[1].Sigma_ss*((1.-Grp[1].Pi)*(in1+fs*fsreg)+Run.Reactor->volratio*Grp[0].Pi*(in0+fs*(1-fsreg)))/denom1))/denom0;
  *phi1=((1.-Grp[1].Pi)*(in1+fs*fsreg)+Run.Reactor->volratio*Grp[0].Pi*(in0+fs*(1-fsreg)+Grp[0].Sigma_ss*0*(*phi0)))/denom1;
  // whew!
}

void selfshield() {

  // calculates self-shielding f factors for all nuclides

  float sigma_t_homo;  // homogenized cross section
  int i, count;

  get_sigmas();   // First calculation of f-factor begins here
  sigma_t_homo=homogenize(Grp[0].Sigma_tr,Grp[1].Sigma_tr,Run.Reactor->volratio);
  is_important(sigma_t_homo);
  get_probs(&Grp[0].Pi,&Grp[1].Pi);
  calc_f_factor();
  count=backup_f();
  get_sigmas();   // Second iteration for f-factor
  sigma_t_homo=homogenize(Grp[0].Sigma_tr,Grp[1].Sigma_tr,Run.Reactor->volratio);
  is_important(sigma_t_homo);
  get_probs(&Grp[0].Pi,&Grp[1].Pi);
  calc_f_factor();
  while (conv_test(count) != 1) {  // subsequent iterations until convergence achieved: rarely necessary
    count=backup_f();
    get_sigmas();
    sigma_t_homo=homogenize(Grp[0].Sigma_tr,Grp[1].Sigma_tr,Run.Reactor->volratio);
    is_important(sigma_t_homo);
    get_probs(&Grp[0].Pi,&Grp[1].Pi);
    calc_f_factor();
  }
  get_sigmas();   // convergence achieved; update Pi_0 and Pi_1 and proceed
  sigma_t_homo=homogenize(Grp[0].Sigma_tr,Grp[1].Sigma_tr,Run.Reactor->volratio);
  get_probs(&Grp[0].Pi,&Grp[1].Pi);
  for(i=0; i<=1; i++) {
    Grp[i].Nuclide=Grp[i].NuclFront;
    while(Grp[i].Nuclide != NULL) {
      Grp[i].Nuclide->f_stored[Run.ncurrent]=Grp[i].Nuclide->f_calc;
      Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    }
  }

}

void freeRunAndReactors()
{
	free(Run.ReactorFront->MatlFront[0]);
    Run.ReactorFront->MatlFront[0] = NULL;
	free(Run.ReactorFront->MatlFront[1]);
    Run.ReactorFront->MatlFront[1] = NULL;
	free(Run.ReactorFront->MatlFront[2]);
    Run.ReactorFront->MatlFront[2] = NULL;
	free(Run.ReactorFront->MatlFront[3]);
    Run.ReactorFront->MatlFront[3] = NULL;
	free(Run.ReactorFront);
	Run.ReactorFront = NULL;
	Run.Reactor = NULL;
}

void Link_reaclist(REACSTR *firstreac) {

  // Link the first element in the reactor list

  Run.ReactorFront=Create_reacnode(firstreac);
  Run.Reactor=Run.ReactorFront;
}

REACSTR *Create_reacnode(REACSTR *reac) {

  // Assign relevant values to this member of the reactor list

     REACSTR *result = (REACSTR *) malloc(sizeof(REACSTR));
     result->volratio = reac->volratio;
     result->r[0] = reac->r[0];
     result->r[1] = reac->r[1];
     result->geomtype = reac->geomtype;
     result->core_r = reac->core_r;
     result->core_h = reac->core_h;
     result->core_r_equiv = reac->core_r_equiv;
     result->Fission_source_region = reac->Fission_source_region;
     result->T[0]=reac->T[0];
     result->T[1]=reac->T[1];
     result->power=reac->power;
     result->inventory=reac->inventory;
     result->pow_den=reac->pow_den;
     result->effic=reac->effic;
     result->load_factor=reac->load_factor;
     result->t_clad=reac->t_clad;
     strcpy(result->Name,reac->Name);
     result->NextReactor = NULL;
     return result;
 }

void Insert_reacnode(REACSTR *place, REACSTR *reac) {

  // Create a link to the next member of the reactor list; call the routine that assigns values

  place->NextReactor = Create_reacnode(reac);
}


void downscatter_source(REGSTR* nucl) {

  // Derives downscattering vectors (fine, coarse) for a nuclide; this runs only once per material

  float delta;  // as defined in section 4.1
  float* palpha=&nucl->alpha;
  float* plethrange=&nucl->lethrange;
  int* pndown=&nucl->ndown;
  int* pndownfine=&nucl->ndownfine;
  //int i,j;
  //float dstally;

  get_alpha(palpha,plethrange,nucl->a);
  delta=DELTAFAST; // Derive downscattering array for this nuclide for the fine group structure
  derive_downscatter(*nucl->downscatterfine,delta,pndownfine,nucl->a,nucl->alpha,nucl->lethrange);
  delta=DELTAFAST*10.;  // Next for the coarse group structure; it's actually quicker to recalculate, rather than adding components of downscatterfine. FIXED downscatter for heavy nuclides
  derive_downscatter(*nucl->downscatter,delta,pndown,nucl->a,nucl->alpha,nucl->lethrange);
}

void augment_inscatterfine() {

  // adds the current group's contribution to the inscattering vector
  int i;

  for(i=0; i<=1; i++) {
    Grp[i].Nuclide=Grp[i].NuclFront;
    while (Grp[i].Nuclide != NULL) {  // go through each nuclide; add its contribution
      derive_inscatterfine(*Run.inscfine,Grp[i].Nuclide->downscatterfine,Grp[i].Nuclide->ndownfine,Run.ncurrent,Grp[i].Nuclide->sigma_s,Grp[i].Nuclide->nd,Grp[i].Nuclide->mu_cm,Run.phifine[Run.ncurrent][i],i);
      if(Grp[i].Nuclide->a > 220 && Run.ncurrent < NHI) augment_inscatter_inel(*Run.inscfine,Grp[i].Nuclide->inel_krnl,Run.ncurrent,Grp[i].Nuclide->sigma_i,Grp[i].Nuclide->nd,Run.phifine[Run.ncurrent][i],i,NHI);
      Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    }
  }
}

void augment_inscatter() {

  // adds the current group's contribution to the inscattering vector

  int i;

  for(i=0; i<=1; i++) {
    Grp[i].Nuclide=Grp[i].NuclFront;
    while (Grp[i].Nuclide != NULL) {
      derive_inscatter(*Run.insc,Run.insc_th,Grp[i].Nuclide->downscatter,Grp[i].Nuclide->ndown,Run.ncurrent,Grp[i].Nuclide->sigma_s,Grp[i].Nuclide->nd,Grp[i].Nuclide->mu_cm,Run.phi[Run.ncurrent][i],i);
      if(Grp[i].Nuclide->a > 220 && Run.ncurrent < NHI/10) augment_inscatter_inel(*Run.insc,Grp[i].Nuclide->inel_krnl_c,Run.ncurrent,Grp[i].Nuclide->sigma_i,Grp[i].Nuclide->nd,Run.phi[Run.ncurrent][i],i,NHI/10);
      Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    }
  }
}

int conv_test(int count) {

  // tests f_calc against f_old for convergence by comparing |f_calc-f_old| to TOL

  float TOL2={0.01*0.01*count*count}, errormag={0.};
  int i;

  for(i=0; i<=1; i++) {
    Grp[i].Nuclide=Grp[i].NuclFront;
    while (Grp[i].Nuclide != NULL) {
      errormag=errormag+((Grp[i].Nuclide->f_calc-Grp[i].Nuclide->f_old)*(Grp[i].Nuclide->f_calc-Grp[i].Nuclide->f_old));
      Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    }
  }
  if (count != 0) {
    if (errormag < TOL2) return 1;
    else return 0;
  }
  else return 1;
}

void InitializeNuclReg(float* inscatter, int NIN) {

  // resets values that may have changed during previous runs

  int i;

  for(i=0; i<NIN; i++) {   // wipe inscattering array
    *(inscatter+2*i)=0.;
    *(inscatter+2*i+1)=0.;
  }
}

void GetRawDataFine(int grp) {

  // loads current data from database into Grp[].Nuclide struct
  int an={0},i;

  for(i=0; i<=1; i++) {
    Grp[i].Nuclide=Grp[i].NuclFront;
    while (Grp[i].Nuclide != NULL) {
      if(Grp[i].Nuclide->a > 220) { //actinide
        put_actinide(Grp[i].Nuclide,an,grp,0);
        an++;
      }
      else {
        put_nonactinide(Grp[i].Nuclide,grp,0);  // nonactinide
      }
      Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    }
  }
}

void GetRawData(int grp) {

  // loads current data from database into Grp[].Nuclide struct
  int i,an={0};

  for(i=0; i<=1; i++) {
    Grp[i].Nuclide=Grp[i].NuclFront;
    while (Grp[i].Nuclide != NULL) {
      if(Grp[i].Nuclide->a > 220) { //actinide
        put_actinide_c(Grp[i].Nuclide,grp,0,an);
        an++;
      }
      else {
        put_nonactinide_c(Grp[i].Nuclide,grp,0);  // nonactinide
      }
      Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    }
  }
}

void GetRawDataTh(int grp) {

  // loads current data from database into Grp[].Nuclide struct
  int i,an={0};

  for(i=0; i<=1; i++) {
    Grp[i].Nuclide=Grp[i].NuclFront;
    while (Grp[i].Nuclide != NULL) {
      if(Grp[i].Nuclide->a > 220) { //actinide
        put_actinide_c(Grp[i].Nuclide,grp,1,an);
        an++;
      }
      else {
        put_nonactinide_c(Grp[i].Nuclide,grp,1);  // nonactinide
      }
      Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    }
  }
}

void put_actinide_c(REGSTR *Reg, int group, int th_or_fast, int index) {

  // puts data for energy group [group] of actinide in the actinide array into REGSTR.  th_or_fast tells it which data set to use: 0=fast, 1=th. this is for the coarse group structure
  int i;

  if (th_or_fast == 0) {  // fast assignments
    Reg->mu_lab=Reg->mu_lab_c[group];
    Reg->sigma_s=Reg->sigma_s_c[group];
    Reg->sigma_g=Reg->sigma_g_c[group];
    Reg->sigma_f=Reg->sigma_f_c[group];
    Reg->nubar=Reg->nubar_c[group];
    Reg->sigma_i=Reg->sigma_i_c[group];
    Reg->xs=Reg->sigma_s+Reg->sigma_g+Reg->sigma_f+Reg->sigma_i;
    if(group < NHI/10) {
      for(i=0; i<NHI/10; i++) {
        Reg->inel_krnl_c[i]=globalinel_c[group][i];
      }
      Reg->inel_ss=Reg->inel_krnl_c[group];
    }
    Reg->f_calc=1.;
  }
  else {   // thermal assignments; will be relevant for csd.  scattering kernel is here.
    Reg->mu_lab=actinide_array[index].th.scos[group];
    Reg->sigma_s=actinide_array[index].th.scat[group];
    Reg->sigma_g=actinide_array[index].th.absp[group];
    Reg->sigma_f=actinide_array[index].th.fiss[group];
    Reg->nubar=actinide_array[index].th.nubr[group];
    Reg->xs=Reg->sigma_s+Reg->sigma_g+Reg->sigma_f;
    for(i=0; i<NTH; i++) {
      Reg->sc_krnl[i]=actinide_array[index].th.sker[i][group];
      //tester+=actinide_array[index].th.sker[group][i];
    }
    //if(tester > actinide_array[index].th.scat[group]*1.001 || tester < actinide_array[index].th.scat[group]*.999) printf("Trouble! %e ",tester/actinide_array[index].th.scat[group]);
  }
}

void put_nonactinide_c(REGSTR *Reg, int group, int th_or_fast) {

  // puts data for energy group [group] of nonactinide into REGSTR.  th_or_fast tells it which data set to use: 0=fast, 1=th. this is for the coarse group structure

  int i;
  double tester=0;

  if (th_or_fast == 0) {  // fast assignments
    Reg->mu_lab=Reg->mu_lab_c[group];
    Reg->sigma_s=Reg->sigma_s_c[group];
    Reg->sigma_g=Reg->sigma_g_c[group];
    Reg->sigma_f=0.;
    Reg->xs=Reg->sigma_s+Reg->sigma_g;
    Reg->f_calc=1.;
  }
  else {   // thermal assignments; will be relevant for csd.  scattering kernel is here.
    Reg->mu_lab=Reg->rawdata->th.scos[group];
    Reg->sigma_s=Reg->rawdata->th.scat[group];
    Reg->sigma_g=Reg->rawdata->th.absp[group];
    Reg->sigma_f=0.;
    Reg->xs=Reg->sigma_s+Reg->sigma_g;
    for(i=0; i<NTH; i++){
      Reg->sc_krnl[i]=Reg->rawdata->th.sker[i][group];
      tester+=Reg->rawdata->th.sker[group][i];
    }
    if(tester > Reg->rawdata->th.scat[group]*1.001 || tester < Reg->rawdata->th.scat[group]*.999) printf("Trouble! %e ",tester/Reg->rawdata->th.scat[group]);
  }
}

int backup_f() {

  // copies current values of f_calc to f_old, counts the number of important nuclides for convergence check

  int i,j={0};

  for(i=0; i<=1; i++) {
    Grp[i].Nuclide=Grp[i].NuclFront;
    while (Grp[i].Nuclide != NULL) {
      Grp[i].Nuclide->f_old=Grp[i].Nuclide->f_calc;
      if (Grp[i].Nuclide->important == 1) j++;
      Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    }
  }
  return (j);
}

void get_tot_nd() {

  // sums the number densities, regions 0 and 1, to find the total number density in the regions (units of 10^24)

  int i;

for(i=0; i<=1; i++) {
    Grp[i].Nuclide=Grp[i].NuclFront;
    Grp[i].ndtot=0.;
    while (Grp[i].Nuclide != NULL) {
      Grp[i].ndtot=Grp[i].ndtot+Grp[i].Nuclide->nd;
      Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    }
  }
}

void Link_nuclreglist(REGSTR *firstnuc, int i) {

  // Link the first element in the nuclide list for region i

  Grp[i].NuclFront=Create_nuclregnode(firstnuc);
  Grp[i].Nuclide=Grp[i].NuclFront;
  Grp[i].Nuclide->PrevNucl=NULL;
}

REGSTR *Create_nuclregnode(REGSTR *nucl) {

  // Assign relevant values to this member of the nuclide list
  int i;

     REGSTR *result = (REGSTR *) malloc(sizeof(REGSTR));
     result->xs = nucl->xs;
     result->nd = nucl->nd;
     result->f_calc = nucl->f_calc;
     result->important = nucl->important;
     result->sigma_s=nucl->sigma_s;
     result->sigma_g=nucl->sigma_g;
     result->sigma_i=nucl->sigma_i;
     result->a=nucl->a;
     result->lambda=nucl->lambda;
     result->nubar=nucl->nubar;
     result->sigma_f=nucl->sigma_f;
     result->mu_lab=nucl->mu_lab;
     result->ca_nd_factor=0.;         // this is updated elsewhere for control absorbers
     for(i=0; i<6; i++) {
       result->f_factor[i]=nucl->f_factor[i];
     }
     result->NextNucl = NULL;
     return result;
 }

REGSTR *Create_empty_nuclregnode(REGSTR *nucl) {

  // Assign relevant values to this member of the nuclide list

     REGSTR *result = (REGSTR *) malloc(sizeof(REGSTR));
     result->NextNucl = NULL;
     return result;
 }

void Insert_nuclregnode(REGSTR *place, REGSTR *nucl) {

  // Create a link to the next and previous members of the nuclide list; call the routine that assigns values

  place->NextNucl = Create_nuclregnode(nucl);
  place->NextNucl->PrevNucl = place;
}

void Insert_empty_nuclregnode(REGSTR *place, REGSTR *nucl) {

  // Create a link to the next and previous members of the nuclide list; call the routine that assigns values

  place->NextNucl = Create_empty_nuclregnode(nucl);
  place->NextNucl->PrevNucl = place;
}

void Link_matlist(COMPSTR *firstmat, int i) {

  // Link the first material in the reactor materials list for region i

  Run.Reactor->MatlFront[i]=Create_matnode(firstmat);
  Run.Reactor->Matl[i]=Run.Reactor->MatlFront[i];
}

COMPSTR *Create_matnode(COMPSTR *matl) {

  // Assign relevant values to this member of the reactor material list

     COMPSTR *result = (COMPSTR *) malloc(sizeof(COMPSTR));
     result->density = matl->density;
     result->volpct = matl->volpct;
     strcpy(result->Name,matl->Name);
     result->NextMatl = NULL;
     return result;
 }

void Insert_matnode(COMPSTR *place, COMPSTR *matl) {

  // Create a link to the next and previous members of the reactor materials list; call the routine that assigns values

  place->NextMatl = Create_matnode(matl);
}

void Free_nucreglist() {

  // Wipes Reg structs and frees memory in preparation for a new run: frees raw data for nonactinides as well

  int i;
  for(i=0; i<=1; i++) {
    Grp[i].Nuclide=Grp[i].NuclFront;
    while(Grp[i].Nuclide->NextNucl != NULL) {
      Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    } // we're now at the end of the list
    while(Grp[i].Nuclide->PrevNucl != NULL) {
      Grp[i].Nuclide=Grp[i].Nuclide->PrevNucl;
      free(Grp[i].Nuclide->rawdata);
      free(Grp[i].Nuclide->NextNucl);
    }  // now at the first element of the list
    free(Grp[i].NuclFront->rawdata);
    free(Grp[i].NuclFront);  // clear the first element too
    Grp[i].Nuclide=Grp[i].NuclFront=NULL;
  }
}



void freeNuclRegList()
{
    int i;
    for(i=0;i<2;i++)
    {
        freeSpecificNuclRegStruct(Grp[i].NuclFront);
        //freeSpecificNuclRegStruct(Grp[i].NuclFront);
		Grp[i].NuclFront->NextNucl = NULL;
		Grp[i].NuclFront->PrevNucl = NULL;
		Grp[i].Nuclide = NULL;
		Grp[i].NuclFront = NULL;
    }
}

void freeSpecificNuclRegStruct(REGSTR *regPtr)
{

  REGSTR *tempA, *tempB;
  tempA = regPtr;
  while(tempA->NextNucl != NULL)
    tempA = tempA->NextNucl;  // seek to end of linked list
  while(tempA->PrevNucl != NULL) // rewind and free one at a time
  {
    tempB = tempA;
    tempA = tempA->PrevNucl;
	  if(tempB != NULL)
	  {
		  tempB->PrevNucl = NULL;
		  tempB->NextNucl = NULL;
		  free(tempB);
	  }
  }
  if(tempA != NULL)
  {
	  tempA->PrevNucl = NULL;
	  tempA->NextNucl = NULL;
	  free(tempA);
  }

}

void LoadReactors() {

  // Loads reactor data from input file created by me: file is not to be altered unless user knows what s/he is doing

  FILE *f;
  REACSTR Reac;  // Dummy that holds data as it's read from file
  char line_read[30];
  int i,j,k,l;
  COMPSTR Material;

  f=fopen("reactors","r");
  Run.Reactor=Run.ReactorFront=NULL;
  fgets(line_read,30,f);
  j=0;
  while(line_read[0] != '#') {  // The character '#' MUST terminate the input
  // Go through some contortions to read a string that may contain spaces...
    i=0;
    while (line_read[i] != '\0') {
      Reac.Name[i]=line_read[i];
      i++;
    }
    Reac.Name[i-1]='\0';
    // Read numerical values according to this format:
    // T[0] T[1] CR power inventory effic LF CR
    fscanf(f,"%f %f\n",&Reac.T[0],&Reac.T[1]);
    fscanf(f,"%f %f %f %f\n",&Reac.power,&Reac.inventory,&Reac.effic,&Reac.load_factor);
    Reac.effic=Reac.effic/100.;
    Reac.load_factor=Reac.load_factor/100.;
    Reac.pow_den=Reac.power/Reac.inventory;
    // r[0] r[1] t_clad CR core_r core_h CR geomtype Fission_region CR
    fscanf(f,"%f %f %f\n",&Reac.r[0],&Reac.r[1],&Reac.t_clad);
    Reac.r[0]=Reac.r[0]+Reac.t_clad;  // we lump cladding with region 0
    fscanf(f,"%f %f\n",&Reac.core_r,&Reac.core_h);
    Reac.core_r_equiv=pow(0.75*Reac.core_r*Reac.core_r*Reac.core_h,1./3.);
    fscanf(f,"%d %d\n",&Reac.geomtype,&Reac.Fission_source_region);
    derive_volratio(Reac.r,&Reac.volratio,Reac.geomtype);
    if(j==0) {
      Link_reaclist(&Reac);
      j=1;
    }
    else {
      Insert_reacnode(Run.Reactor,&Reac);
      Run.Reactor=Run.Reactor->NextReactor;
    }
   // read materials in region k
    for(k=0; k<=1; k++) {
      l=0;
      fgets(line_read,30,f);
      // according to format NAME CR density v/o CR * CR next...
      while(line_read[0] != '*') {  // The character '*' MUST terminate the list
        // Go through some contortions to read a string that may contain spaces...
        i=0;
        while (line_read[i] != '\0') {
          Material.Name[i]=line_read[i];
          i++;
        }
        Material.Name[i-1]='\0';
        fscanf(f,"%f %f\n",&Material.density,&Material.volpct);
        Material.volpct=Material.volpct/100.;
        if(l==0) {
          Link_matlist(&Material,k);
          l=1;
        }
        else {
          Insert_matnode(Run.Reactor->Matl[k],&Material);
          Run.Reactor->Matl[k]=Run.Reactor->Matl[k]->NextMatl;
        }
        fgets(line_read,30,f);
      }
      l=0;
    }
    fgets(line_read,30,f);  // next read in control absorber(s)
    while(line_read[0] != '*') {  // The character '*' MUST terminate the list
        // Go through some contortions to read a string that may contain spaces...
      i=0;
      while (line_read[i] != '\0') {
        Material.Name[i]=line_read[i];
        i++;
      }
      Material.Name[i-1]='\0';
      Material.density=0.;
      Material.volpct=0.;
      if(l==0) {
        Link_matlist(&Material,2);
        l=1;
      }
      else {
        Insert_matnode(Run.Reactor->Matl[2],&Material);
        Run.Reactor->Matl[2]=Run.Reactor->Matl[2]->NextMatl;
      }
      fgets(line_read,30,f);
    }
    l=0;
    fgets(line_read,30,f); // now read cladding materal
    i=0;
    while (line_read[i] != '\0') {
      Run.Reactor->CladName[i]=line_read[i];
        i++;
      }
    Run.Reactor->CladName[i-1]='\0';
    fgets(line_read,30,f); // now read structural materal
    i=0;
    while (line_read[i] != '\0') {
      Material.Name[i]=line_read[i];
        i++;
      }
    Material.Name[i-1]='\0';
    fscanf(f,"%f %f\n",&Material.density,&Material.volpct);  // strucural material, density and volume % of reactor
    Material.volpct=Material.volpct/100.;
    Link_matlist(&Material,3);
    fgets(line_read,30,f);
  }
}

void put_actinide(REGSTR *Reg, int index, int group, int th_or_fast) {

  // puts data for energy group [group] of actinide [index] in the actinide array into REGSTR.  th_or_fast tells it which data set to use: 0=fast, 1=th

  int i,j;

  if (th_or_fast == 0) {  // fast assignments
    Reg->mu_lab=actinide_array[index].lh.scos[group];
    Reg->sigma_s=actinide_array[index].lh.scat[group];
    Reg->sigma_g=actinide_array[index].lh.absp[group];
    Reg->sigma_f=actinide_array[index].lh.fiss[group];
    Reg->nubar=actinide_array[index].lh.nubr[group];
    Reg->sigma_i=actinide_array[index].lh.inxs[group];
    if(group < NHI) {
      for(i=0; i<NHI; i++) {
        Reg->inel_krnl[i]=globalinel[group][i];
      }
      Reg->inel_ss=Reg->inel_krnl[group];
    }
    Reg->xs=Reg->sigma_s+Reg->sigma_g+Reg->sigma_f+Reg->sigma_i;
    for(j=0; j<6; j++) {
      Reg->f_factor[j]=actinide_array[index].lh.ffct[group][j];
    }
  }
  else {   // thermal assignments; will be relevant for csd.  scattering kernel is not here yet.
    Reg->mu_lab=actinide_array[index].th.scos[group];
    Reg->sigma_s=actinide_array[index].th.scat[group];
    Reg->sigma_g=actinide_array[index].th.absp[group];
    Reg->sigma_f=actinide_array[index].th.fiss[group];
      Reg->nubar=actinide_array[index].th.nubr[group];
    Reg->xs=Reg->sigma_s+Reg->sigma_g+Reg->sigma_f;
  }
}

void put_nonactinide(REGSTR *Reg, int group, int th_or_fast) {

  // puts data for energy group [group] of nonactinide into REGSTR.  th_or_fast tells it which data set to use: 0=fast, 1=th

  int j;

  if (th_or_fast == 0) {  // fast assignments
    Reg->mu_lab=Reg->rawdata->lh.scos[group];
    Reg->sigma_s=Reg->rawdata->lh.scat[group];
    Reg->sigma_g=Reg->rawdata->lh.absp[group];
    Reg->sigma_f=0.;
    Reg->nubar=0.;
    Reg->xs=Reg->sigma_s+Reg->sigma_g;
    for(j=0; j<6; j++) {
      Reg->f_factor[j]=Reg->rawdata->lh.ffct[group][j];
    }
  }
  else {   // thermal assignments; will be relevant for csd.  scattering kernel is not here yet.
    Reg->mu_lab=Reg->rawdata->th.scos[group];
    Reg->sigma_s=Reg->rawdata->th.scat[group];
    Reg->sigma_g=Reg->rawdata->th.absp[group];
    Reg->sigma_f=0.;
    Reg->nubar=0.;
    Reg->xs=Reg->sigma_s+Reg->sigma_g;
  }
}

void assign_nonactinide(REGSTR *Reg) {

  // assigns misc data to a nonactinide

  Reg->a=Reg->rawdata->a;
  Reg->lambda=0.;

}

void assign_actinide(int index, REGSTR *Reg) {

  // assigns misc. data (A, lambda, etc.) to an actinide

  Reg->a=actinide_array[index].a;
  Reg->lambda=actinide_array[index].lambda;
}


float LoadNuclReg()

     // This routine parses the reactor data to construct lists of nuclides in each region

{
  REGSTR Reg;

  int i,j,freg;  // freg=fuel region
  float bound_amu;  // atomic wt. of non-fuel components of fuel molecule: e.g. for UO_2 bound_amu=2*16=32
  int soluble_ca_present_in_region;  // = 0 or 1 if absorber concentration in ppm specified for region 0 or 1; -1 otherwise
  float soluble_ca_conc; // in ppm
  float ca_region_nd;  // number density of non-ca components in liquid region

  soluble_ca_present_in_region = -1;
  soluble_ca_conc=0.;
  ca_region_nd=0.;
	// find location of control absorber if present
  for(i=0; i<=1; i++) {
    Run.Reactor->Matl[i]=Run.Reactor->MatlFront[i];
    while(Run.Reactor->Matl[i] != NULL) {   // determine if there's a soluble CA and its location
      if(Run.Reactor->Matl[i]->density < 0.) {
        soluble_ca_present_in_region=i;
        break;
      }
      Run.Reactor->Matl[i]=Run.Reactor->Matl[i]->NextMatl;
    }
    if(soluble_ca_present_in_region > -1) break;
  }
  freg=Run.Reactor->Fission_source_region;

	// for Grp[fission region], put actinides in the REGSTR then copy to Grp[freg].Nuclide
	// and repeat for the 23 actinides
	// Reg is essentially a temp variable (well, reused).  REGSTR does not refer to a region
	// as such, it is the struct containing region-specific info for a nuclide, eg self-shielding
	// data which changes for an actinide in different regions.
  for(i=0; i<NACT; i++) {  // put the actinides in the first 23 spots in the appropriate region (usu. 0)
     // load cross sections etc from the global store into the region struct
	  put_actinide(&Reg,i,0,0);
	  // set atomic mass and lambda of Reg
     assign_actinide(i,&Reg);
    if(i==0) Link_nuclreglist(&Reg,freg);
    else {

      Insert_nuclregnode(Grp[freg].Nuclide,&Reg);
      printf("freg = %d, i = %d\n", freg, i);
      Grp[freg].Nuclide=Grp[freg].Nuclide->NextNucl;
    }
    sprintf(Grp[freg].Nuclide->prn_name,"%s", act_basenames[i]);
    Grp[freg].Nuclide->nd=0.;  // make sure nd is clean
  }
  Insert_empty_nuclregnode(Grp[freg].Nuclide,&Reg);
  Grp[freg].Nuclide=Grp[freg].Nuclide->NextNucl;
  Grp[freg].Nuclide->rawdata=get_nonactinide("lumped_fp",300.1,NULL, &nonActArray[0]);  // lumped FP data *always* follows the 23 actinide structs
  assign_nonactinide(Grp[freg].Nuclide);
  Grp[freg].Nuclide->mu_lab=2./3./Grp[freg].Nuclide->a;   // fission products experience s-wave scattering only
  Grp[freg].Nuclide->nd=0.;
  sprintf(Grp[freg].Nuclide->prn_name,"lumped FP");
  // Equilibrium Xe and Sm *always* follow the lumped FP
  Insert_empty_nuclregnode(Grp[freg].Nuclide,&Reg);
  Grp[freg].Nuclide=Grp[freg].Nuclide->NextNucl;
  Grp[freg].Nuclide->rawdata=get_nonactinide("xe-135",Run.Reactor->T[freg],NULL, &nonActArray[1]);
  assign_nonactinide(Grp[freg].Nuclide);
  sprintf(Grp[freg].Nuclide->prn_name,"xe-135");
  Grp[freg].Nuclide->nd=0.;  // this will be updated a bit later, once we have an idea of the flux
  Insert_empty_nuclregnode(Grp[freg].Nuclide,&Reg);
  Grp[freg].Nuclide=Grp[freg].Nuclide->NextNucl;
  Grp[freg].Nuclide->rawdata=get_nonactinide("sm-149",Run.Reactor->T[freg],NULL, &nonActArray[2]);
  assign_nonactinide(Grp[freg].Nuclide);
  sprintf(Grp[freg].Nuclide->prn_name,"sm-149");
  Grp[freg].Nuclide->nd=0.;  // this will be updated a bit later, once we have an idea of the flux
  // we've finished loading the actinides and their hangers-on (lumped fission product, equilibrium Xe and Sm).  Now we parse the Run.Reactor data to assemble the rest of the stuff: first control absorber(s), uniformly distributed in both regions.
  Run.Reactor->Matl[2]=Run.Reactor->MatlFront[2];
  while(Run.Reactor->Matl[2] != NULL) {
    if(strcmp(Run.Reactor->Matl[2]->Name,"b4c")==0) {
      // boron carbide: we just assume it's all b-10, neglecting the carbon
      for(i=0; i<=1; i++) {
        if(Grp[i].Nuclide==NULL) {
          Link_nuclreglist(&Reg,i);
        }
        else {
          Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
          Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        }
        Grp[i].Nuclide->rawdata=get_nonactinide("b-10",Run.Reactor->T[i],NULL, &nonActArray[3]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"B-10 in B4C");
        Grp[i].Nuclide->ca_nd_factor=1.;   // number density factor indicates it's a control absorber and thus subject to adjustment in criticality search routine.
      }
    }
    else if(strcmp(Run.Reactor->Matl[2]->Name,"ag-in-cd")==0) {
     // ag-in-cd: number density ratio ag-107:ag-109:in-0:cd-0 0.5:0.5:1.0:1.0
      for(i=0; i<=1; i++) {
        if(Grp[i].Nuclide==NULL) {
          Link_nuclreglist(&Reg,i);
        }
        else {
          Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
          Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        }
        Grp[i].Nuclide->rawdata=get_nonactinide("ag-107",Run.Reactor->T[i],NULL, &nonActArray[4]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"Ag-107 in Ag-In-Cd");
        Grp[i].Nuclide->ca_nd_factor=0.5;   // number density factor indicates it's a control absorber and thus subject to adjustment in criticality search routine.
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("ag-109",Run.Reactor->T[i],NULL, &nonActArray[5]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"Ag-109 in Ag-In-Cd");
        Grp[i].Nuclide->ca_nd_factor=0.5;   // number density factor indicates it's a control absorber and thus subject to adjustment in criticality search routine.
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("in-0",Run.Reactor->T[i],NULL, &nonActArray[6]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"In in Ag-In-Cd");
        Grp[i].Nuclide->ca_nd_factor=1.0;
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("cd-0",Run.Reactor->T[i],NULL, &nonActArray[7]);
        assign_nonactinide(Grp[i].Nuclide);
        Grp[i].Nuclide->ca_nd_factor=1.0;
        sprintf(Grp[i].Nuclide->prn_name,"Cd in Ag-In-Cd");
      }
    }
    else if(strcmp(Run.Reactor->Matl[2]->Name,"gd2o3")==0) {
      // gadolinium oxide: ignore the oxygen, number density ratios gd-155:gd-157 0.5:0.5; other isotopes of gd are neglected
      for(i=0; i<=1; i++) {
        if(Grp[i].Nuclide==NULL) {
          Link_nuclreglist(&Reg,i);
        }
        else {
          Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
          Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        }
        Grp[i].Nuclide->rawdata=get_nonactinide("gd-155",Run.Reactor->T[i],NULL, &nonActArray[8]);
        assign_nonactinide(Grp[i].Nuclide);
        Grp[i].Nuclide->ca_nd_factor=0.5;
        sprintf(Grp[i].Nuclide->prn_name,"Gd-155 in Gd2O3");
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("gd-157",Run.Reactor->T[i],NULL, &nonActArray[9]);
        assign_nonactinide(Grp[i].Nuclide);
        Grp[i].Nuclide->ca_nd_factor=0.5;
        sprintf(Grp[i].Nuclide->prn_name,"Gd-157 in Gd2O3");
      }
    }
    else standard_error("Invalid control absorber in Reactor");
    Run.Reactor->Matl[2]=Run.Reactor->Matl[2]->NextMatl;
  }
  // control absorbers are in. next, structural material, admixed uniformly according to its volume fraction and density
  Run.Reactor->Matl[3]=Run.Reactor->MatlFront[3];
  while(Run.Reactor->Matl[3] != NULL) {
          if(strcmp(Run.Reactor->Matl[3]->Name,"steel")==0) {  // steel, use fe-56 for this
      for(i=0; i<=1; i++) {
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("fe-56",Run.Reactor->T[i],NULL, &nonActArray[10]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"Structural Fe");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[3]->density*N_AVO/56.*Run.Reactor->Matl[3]->volpct;  // number density = density [g/cm^3] * #/g [unit of 10^24] * (small) volume fraction occupied by steel
      }
    }
    else standard_error("Invalid structural material in Reactor: this is a socialist program: the decision has been made for you: you have no choice but to enter 'steel'");
    Run.Reactor->Matl[3]=Run.Reactor->Matl[3]->NextMatl;
  }
  // structural material is in. next, cladding, admixed in region 0
  if(strcmp(Run.Reactor->CladName,"zircaloy")==0) {  // zircaloy, Zr-0 at its natural number density, mixed in fuel pin according to vol. fraction of pin it occupies
    Insert_empty_nuclregnode(Grp[0].Nuclide,&Reg);
    Grp[0].Nuclide=Grp[0].Nuclide->NextNucl;
    Grp[0].Nuclide->rawdata=get_nonactinide("zr-0",Run.Reactor->T[0],NULL, &nonActArray[11]);
    assign_nonactinide(Grp[0].Nuclide);
    sprintf(Grp[0].Nuclide->prn_name,"Zr in Clad");
    Grp[0].Nuclide->nd=Run.Reactor->clad_rho*N_AVO/91.*clad_vol_frac();  // number density = density [g/cm^3] * [#/g] * (small) fraction of region 0 volume occupied by cladding
  }  else { // Modified 11/11/05: default to void cladding region.  Still add a material node for the cladding, just make the number density zero.
    Insert_empty_nuclregnode(Grp[0].Nuclide,&Reg);
    Grp[0].Nuclide=Grp[0].Nuclide->NextNucl;
    Grp[0].Nuclide->rawdata=get_nonactinide("zr-0",Run.Reactor->T[0],NULL, &nonActArray[12]);
    assign_nonactinide(Grp[0].Nuclide);
    sprintf(Grp[0].Nuclide->prn_name,"Zr in Clad");
    Grp[0].Nuclide->nd=0.;  // number density = density [g/cm^3] * [#/g] * (small) fraction of region 0 volume occupied by cladding
  }
  //standard_error("Invalid cladding material in Reactor");
  // now load in all the other stuff in regions 0 and 1: fuel matrix, moderator, coolant, etc. as specified in reactors file
  for(i=0; i<=1; i++) {
    Run.Reactor->Matl[i]=Run.Reactor->MatlFront[i];
    while(Run.Reactor->Matl[i] != NULL) {
           printf("Adding %s ... \n",Run.Reactor->Matl[i]->Name);
      if(strcmp(Run.Reactor->Matl[i]->Name,"h2o")==0) {  // light water: hh2o + o
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("h-1",Run.Reactor->T[i],"hh2o", &nonActArray[13]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"H in H2O");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/9.*Run.Reactor->Matl[i]->volpct; // number density
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("o-16",Run.Reactor->T[i],NULL, &nonActArray[14]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"O in H2O");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/18.*Run.Reactor->Matl[i]->volpct; // number density
        if(soluble_ca_present_in_region==i) ca_region_nd += Grp[i].Nuclide->nd;
      }
      else if(strcmp(Run.Reactor->Matl[i]->Name,"graphite")==0) {  // graphite: c-12.cgr
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("c-12",Run.Reactor->T[i],"cgr", &nonActArray[15]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"C-12 (Graphite)");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/12.*Run.Reactor->Matl[i]->volpct;
        if(soluble_ca_present_in_region==i) ca_region_nd += Grp[i].Nuclide->nd;
      }
      else if(strcmp(Run.Reactor->Matl[i]->Name,"hydrogen")==0) {  // hydrogen: h-1
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("h-1",Run.Reactor->T[i],NULL, &nonActArray[15]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"H-1");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/1.*Run.Reactor->Matl[i]->volpct;
        if(soluble_ca_present_in_region==i) ca_region_nd += Grp[i].Nuclide->nd;
      }
      else if(strcmp(Run.Reactor->Matl[i]->Name,"deuterium")==0) {  // hydrogen: h-2
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("h-2",Run.Reactor->T[i],NULL, &nonActArray[16]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"H-2");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/2.*Run.Reactor->Matl[i]->volpct;
        if(soluble_ca_present_in_region==i) ca_region_nd += Grp[i].Nuclide->nd;
      }
      else if(strcmp(Run.Reactor->Matl[i]->Name,"d2o")==0) {  // heavy water: dd2o + o
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("h-2",Run.Reactor->T[i],"dd2o", &nonActArray[17]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"D in D2O");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/10.*Run.Reactor->Matl[i]->volpct;
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("o-16",Run.Reactor->T[i],NULL, &nonActArray[18]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"O in D2O");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/20.*Run.Reactor->Matl[i]->volpct;
        if(soluble_ca_present_in_region==i) ca_region_nd += Grp[i].Nuclide->nd;
      }
      else if(strcmp(Run.Reactor->Matl[i]->Name,"he-4")==0) {  // helium-4
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("he-4",Run.Reactor->T[i],NULL, &nonActArray[19]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"He-4");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/4.*Run.Reactor->Matl[i]->volpct;
        if(soluble_ca_present_in_region==i) ca_region_nd += Grp[i].Nuclide->nd;
      }
      else if(strcmp(Run.Reactor->Matl[i]->Name,"lead")==0) {  // pb-206,207,208 in ratio .25:.22:.53
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("pb-206",Run.Reactor->T[i],NULL, &nonActArray[20]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"Pb-206");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/207.*Run.Reactor->Matl[i]->volpct*.25;
        if(soluble_ca_present_in_region==i) ca_region_nd += Grp[i].Nuclide->nd;
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("pb-207",Run.Reactor->T[i],NULL, &nonActArray[21]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"Pb-207");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/207.*Run.Reactor->Matl[i]->volpct*.22;
        if(soluble_ca_present_in_region==i) ca_region_nd += Grp[i].Nuclide->nd;
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("pb-208",Run.Reactor->T[i],NULL, &nonActArray[22]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"Pb-208");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/208.*Run.Reactor->Matl[i]->volpct*.53;
        if(soluble_ca_present_in_region==i) ca_region_nd += Grp[i].Nuclide->nd;
      }
      else if(strcmp(Run.Reactor->Matl[i]->Name,"boron")==0) {  // {soluble} boron-10, 11 in ratio .2:.8
        if(Run.Reactor->Matl[i]->density > 0.) {
           Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
           Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
           Grp[i].Nuclide->rawdata=get_nonactinide("b-10",Run.Reactor->T[i],NULL, &nonActArray[23]);
           assign_nonactinide(Grp[i].Nuclide);
           sprintf(Grp[i].Nuclide->prn_name,"B-10");
           Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/10.2*Run.Reactor->Matl[i]->volpct*.2;
           if(soluble_ca_present_in_region==i) ca_region_nd += Grp[i].Nuclide->nd;
           Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
           Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
           Grp[i].Nuclide->rawdata=get_nonactinide("b-11",Run.Reactor->T[i],NULL, &nonActArray[24]);
           assign_nonactinide(Grp[i].Nuclide);
           sprintf(Grp[i].Nuclide->prn_name,"B-11");
           Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/10.2*Run.Reactor->Matl[i]->volpct*.8;
           if(soluble_ca_present_in_region==i) ca_region_nd += Grp[i].Nuclide->nd;
        }
        else {
          soluble_ca_conc=-Run.Reactor->Matl[i]->density;
        }
      }
      else if(strcmp(Run.Reactor->Matl[i]->Name,"bismuth")==0) {  // bi-209
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("bi-209",Run.Reactor->T[i],NULL, &nonActArray[25]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"Bi-209");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/209.*Run.Reactor->Matl[i]->volpct;
        if(soluble_ca_present_in_region==i) ca_region_nd += Grp[i].Nuclide->nd;
      }
      else if(strcmp(Run.Reactor->Matl[i]->Name,"sodium")==0) {  // na-23
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("na-23",Run.Reactor->T[i],NULL, &nonActArray[26]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"Sodium");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/23.*Run.Reactor->Matl[i]->volpct;
        if(soluble_ca_present_in_region==i) ca_region_nd += Grp[i].Nuclide->nd;
      }
      else if(strcmp(Run.Reactor->Matl[i]->Name,"zirconium")==0) {  // zr-0
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("zr-0",Run.Reactor->T[i],NULL, &nonActArray[27]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"Zirconium");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/91.*Run.Reactor->Matl[i]->volpct;
        if(soluble_ca_present_in_region==i) ca_region_nd += Grp[i].Nuclide->nd;
      }
      else if(strcmp(Run.Reactor->Matl[i]->Name,"nb")==0) {  // nb-93
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("nb-93",Run.Reactor->T[i],NULL, &nonActArray[28]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"Niobium");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/93.*Run.Reactor->Matl[i]->volpct;
        if(soluble_ca_present_in_region==i) ca_region_nd += Grp[i].Nuclide->nd;
      }
      else if(strcmp(Run.Reactor->Matl[i]->Name,"co2")==0) {  // carbon dioxide: c + 2o
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("c-12",Run.Reactor->T[i],NULL, &nonActArray[29]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"C-12 in CO2");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/44.*Run.Reactor->Matl[i]->volpct;
        if(soluble_ca_present_in_region==i) ca_region_nd += Grp[i].Nuclide->nd;
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("o-16",Run.Reactor->T[i],NULL, &nonActArray[30]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"O-16 in CO2");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/22.*Run.Reactor->Matl[i]->volpct;
      }
      else if(strcmp(Run.Reactor->Matl[i]->Name,"zro2")==0) {  // inert zro2 matrix: zr + 2o
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("zr-0",Run.Reactor->T[i],NULL, &nonActArray[31]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"Zr in ZrO2");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/123.*Run.Reactor->Matl[i]->volpct;
        if(soluble_ca_present_in_region==i) ca_region_nd += Grp[i].Nuclide->nd;
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("o-16",Run.Reactor->T[i],NULL, &nonActArray[32]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"O in ZrO2");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/61.5*Run.Reactor->Matl[i]->volpct;
      }
      else if(strcmp(Run.Reactor->Matl[i]->Name,"y2o3")==0) {  // inert y2o3 matrix: 2y + 3o
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("y-89",Run.Reactor->T[i],NULL, &nonActArray[33]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"Y in Y2O3");
        Grp[i].Nuclide->nd=2.*Run.Reactor->Matl[i]->density*N_AVO/226.*Run.Reactor->Matl[i]->volpct;
        if(soluble_ca_present_in_region==i) ca_region_nd += Grp[i].Nuclide->nd;
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("o-16",Run.Reactor->T[i],NULL, &nonActArray[34]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"O in Y2O3");
        Grp[i].Nuclide->nd=3.*Run.Reactor->Matl[i]->density*N_AVO/226.*Run.Reactor->Matl[i]->volpct;
      }
      else if(strcmp(Run.Reactor->Matl[i]->Name,"mgo")==0) {  // MgO matrix: mg + h
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("mg-0",Run.Reactor->T[i],NULL, &nonActArray[35]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"Mg in MgO");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/40.3*Run.Reactor->Matl[i]->volpct;
        if(soluble_ca_present_in_region==i) ca_region_nd += Grp[i].Nuclide->nd;
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("o-16",Run.Reactor->T[i],NULL, &nonActArray[36]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"O in MgO");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/40.3*Run.Reactor->Matl[i]->volpct;
      }
      else if(strcmp(Run.Reactor->Matl[i]->Name,"zrh")==0) {  // inert zrh matrix: zr + h
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("zr-0",Run.Reactor->T[i],NULL, &nonActArray[37]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"Zr in ZrH");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/94.*Run.Reactor->Matl[i]->volpct;
        if(soluble_ca_present_in_region==i) ca_region_nd += Grp[i].Nuclide->nd;
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("h-1",Run.Reactor->T[i],"hzrh", &nonActArray[38]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"H in ZrH");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/94*Run.Reactor->Matl[i]->volpct;
      }
      else if(strcmp(Run.Reactor->Matl[i]->Name,"oxidefuel")==0) {  // (Actinide)O_2 fuel
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("o-16",Run.Reactor->T[i],NULL, &nonActArray[39]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"O in Oxide Fuel");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/135.*Run.Reactor->Matl[i]->volpct;
        if(soluble_ca_present_in_region==i) ca_region_nd += Grp[i].Nuclide->nd/2.;
        Run.Reactor->fuel_density=Run.Reactor->Matl[i]->density*Run.Reactor->Matl[i]->volpct;
        bound_amu=32.;
      }
      else if(strcmp(Run.Reactor->Matl[i]->Name,"carbidefuel")==0) {  // (Actinide)C_2 fuel
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("c-12",Run.Reactor->T[i],NULL, &nonActArray[40]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"C in Carbide Fuel");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/131.*Run.Reactor->Matl[i]->volpct;
        if(soluble_ca_present_in_region==i) ca_region_nd += Grp[i].Nuclide->nd/2.;
        Run.Reactor->fuel_density=Run.Reactor->Matl[i]->density*Run.Reactor->Matl[i]->volpct;
        bound_amu=24.;
      }
      else if(strcmp(Run.Reactor->Matl[i]->Name,"nitridefuel")==0) {  // (Actinide)N fuel
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("n-14",Run.Reactor->T[i],NULL, &nonActArray[41]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"N in Nitride Fuel");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/252.*Run.Reactor->Matl[i]->volpct;
        if(soluble_ca_present_in_region==i) ca_region_nd += Grp[i].Nuclide->nd;
        Run.Reactor->fuel_density=Run.Reactor->Matl[i]->density*Run.Reactor->Matl[i]->volpct;
        bound_amu=14.;
      }
      else if(strcmp(Run.Reactor->Matl[i]->Name,"oxycarbidefuel")==0) {  // (Actinide)CO fuel
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("c-12",Run.Reactor->T[i],NULL, &nonActArray[42]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"C in Oxycarbide Fuel");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/266.*Run.Reactor->Matl[i]->volpct;
        if(soluble_ca_present_in_region==i) ca_region_nd += Grp[i].Nuclide->nd;
        Run.Reactor->fuel_density=Run.Reactor->Matl[i]->density*Run.Reactor->Matl[i]->volpct;
        bound_amu=28.;
        Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
        Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
        Grp[i].Nuclide->rawdata=get_nonactinide("o-16",Run.Reactor->T[i],NULL, &nonActArray[43]);
        assign_nonactinide(Grp[i].Nuclide);
        sprintf(Grp[i].Nuclide->prn_name,"O in Oxycarbide Fuel");
        Grp[i].Nuclide->nd=Run.Reactor->Matl[i]->density*N_AVO/266.*Run.Reactor->Matl[i]->volpct;
      }
      else if(strcmp(Run.Reactor->Matl[i]->Name,"fuel")==0) {  // metallic fuel
        Run.Reactor->fuel_density=Run.Reactor->Matl[i]->density*Run.Reactor->Matl[i]->volpct;
        bound_amu=0.;
      }
      else standard_error("Invalid material in Reactor");
      Run.Reactor->Matl[i]=Run.Reactor->Matl[i]->NextMatl;
    }
  }
  if(soluble_ca_present_in_region > -1) {
    i=soluble_ca_present_in_region;
    Grp[i].Nuclide=Grp[i].NuclFront;
    while(Grp[i].Nuclide->NextNucl != NULL) {
      Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    }
    ca_region_nd=ca_region_nd*soluble_ca_conc/1.e6;
    Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
    Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    Grp[i].Nuclide->rawdata=get_nonactinide("b-10",Run.Reactor->T[i],NULL, &nonActArray[44]);
    assign_nonactinide(Grp[i].Nuclide);
    sprintf(Grp[i].Nuclide->prn_name,"B-10 in Shim");
    Grp[i].Nuclide->nd=ca_region_nd*.2;
    Insert_empty_nuclregnode(Grp[i].Nuclide,&Reg);
    Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    Grp[i].Nuclide->rawdata=get_nonactinide("b-11",Run.Reactor->T[i],NULL, &nonActArray[45]);
    assign_nonactinide(Grp[i].Nuclide);
    sprintf(Grp[i].Nuclide->prn_name,"B-11 in Shim");
    Grp[i].Nuclide->nd=ca_region_nd*.8;
  }
  return(bound_amu);
}

void VBUDS_load_fuel(int reg, float rho, float a_matrix, float *actinides_in) {

  // assigns actinide number densities based upon fuel density; generates an initial guess of control absorber concentration based upon fissile fuel number density

  int i,j;
  float ca_nd_guess={0.}, n_total={0.};
  float mass_array[NACT];

  n_total=0.;
  Grp[reg].Nuclide=Grp[reg].NuclFront;
  for(i=0; i<NACT; i++) {
    mass_array[i]=actinides_in[i];
    n_total=n_total+mass_array[i];
    Grp[reg].Nuclide=Grp[reg].Nuclide->NextNucl;
  }
  if(n_total < 0.) {
     n_total=-n_total;
  }  else {
     n_total=0.;
     Grp[reg].Nuclide=Grp[reg].NuclFront;
     for(i=0; i<NACT; i++) {
    	mass_array[i]=actinides_in[i]*(Grp[reg].Nuclide->a+a_matrix)/Grp[reg].Nuclide->a;
    	n_total=n_total+mass_array[i];
    	Grp[reg].Nuclide=Grp[reg].Nuclide->NextNucl;
     }
  }
  Grp[reg].Nuclide=Grp[reg].NuclFront;
  for(i=0; i<NACT; i++) {  // assign number density to each actinide based upon mass or user specified ND
     if(mass_array[i] < 0.) {   // specified as number densities already.
        Grp[reg].Nuclide->nd=-mass_array[i];
     } else {
        Grp[reg].Nuclide->nd=rho*N_AVO/(a_matrix+Grp[reg].Nuclide->a)*mass_array[i]/n_total;
     }
     if (i==I_U_233 || i==I_U_235 || i==I_PU_239 || i==I_PU_241) ca_nd_guess=ca_nd_guess+Grp[reg].Nuclide->nd/9.;   // control absorber concentration guess
     Grp[reg].Nuclide=Grp[reg].Nuclide->NextNucl;
  }
  ca_nd_guess=0.;
  for(i=0; i<3; i++) { // skip ahead thru lumped fp and Xe&Sm to control absorbers
    Grp[reg].Nuclide=Grp[reg].Nuclide->NextNucl;
  }
  Grp[1-reg].Nuclide=Grp[1-reg].NuclFront;
  while(Grp[reg].Nuclide->ca_nd_factor > 0.) {  // assign ca number densities
	  printf("region %d, nuclide name = %s, factor = %f\n", reg, Grp[reg].Nuclide->prn_name, Grp[reg].Nuclide->ca_nd_factor);
	  printf("region %d, nuclide name = %s, factor = %f\n",  1-reg, Grp[1-reg].Nuclide->prn_name, 0.0);
    Grp[reg].Nuclide->nd=ca_nd_guess;
    Grp[1-reg].Nuclide->nd=ca_nd_guess;
    Grp[reg].Nuclide=Grp[reg].Nuclide->NextNucl;
    Grp[1-reg].Nuclide=Grp[1-reg].Nuclide->NextNucl;

  }
  for(i=0; i<=1; i++) {
    Grp[i].Nuclide=Grp[i].NuclFront;
    printf("Region %d\n",i);
    while(Grp[i].Nuclide != NULL) {
                for(j=0; j<NLOHI; j++) {
                        Grp[i].Nuclide->downscatterfine[j][0]=0.;
                        Grp[i].Nuclide->downscatterfine[j][1]=0.;
                }
                for(j=0; j<NLOHI/10; j++) {
                        Grp[i].Nuclide->downscatter[j][0]=0.;
                        Grp[i].Nuclide->downscatter[j][1]=0.;
                }
      downscatter_source(Grp[i].Nuclide);
      printf("%s %e\n",Grp[i].Nuclide->prn_name,Grp[i].Nuclide->nd);
      Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    }
  }
}

void load_fuel(int reg, float rho, float a_matrix) {

  // This subroutine is NOT USED in V:BUDS graphical interface.
  // assigns actinide number densities based upon fuel density; generates an initial guess of control absorber concentration based upon fissile fuel number density

  int i;
  float ca_nd_guess={0.};

  FrontEnd.MTotal=0.;
  for(i=0; i<NACT; i++) {
// BUC 1-A burnup calc
/*     if (i==I_U_235) FrontEnd.Mass[i]=-8.4209e-4*.9; */
/*     else if (i==I_U_234) FrontEnd.Mass[i]=-7.5174e-6*.9; */
/*     else if (i==I_U_238) FrontEnd.Mass[i]=-2.2254e-2*.9; */
/*     else if(i==I_U_236) FrontEnd.Mass[i]=-3.7268e-6*.9; */
// BUC 1-A BU 0
/*     if (i==I_U_235) FrontEnd.Mass[i]=-8.4209e-4*.9; */
/*     else if (i==I_U_234) FrontEnd.Mass[i]=-7.5174e-6*.9; */
/*     else if (i==I_U_238) FrontEnd.Mass[i]=-2.2254e-2*.9; */
/*     else if(i==I_U_236) FrontEnd.Mass[i]=-3.7268e-6*.9; */
// BUC 1-A BU 30
/*     if(i==I_U_234) FrontEnd.Mass[i]=-4.8852e-6*.9; */
/*     else if(i==I_U_235) FrontEnd.Mass[i]=-2.8372e-4*.9; */
/*     else if(i==I_U_236) FrontEnd.Mass[i]=-9.9343e-5*.9; */
/*     else if(i==I_U_238) FrontEnd.Mass[i]=-2.176e-2*.9; */
/*     else if(i==I_NP_237) FrontEnd.Mass[i]=-1.0252e-5*.9; */
/*     else if(i==I_PU_238) FrontEnd.Mass[i]=-2.8474e-6*.9; */
/*     else if(i==I_PU_239) FrontEnd.Mass[i]=-1.3523e-4*.9; */
/*     else if(i==I_PU_240) FrontEnd.Mass[i]=-4.2909e-5*.9; */
/*     else if(i==I_PU_241) FrontEnd.Mass[i]=-2.8069e-5*.9; */
/*     else if(i==I_PU_242) FrontEnd.Mass[i]=-7.4128e-6*.9; */
/*     else if(i==I_AM_241) FrontEnd.Mass[i]=-2.0558e-6*.9; */
/*     else if(i==I_AM_243) FrontEnd.Mass[i]=-1.3655e-6*.9; */
// BUC 1-A BU 40
/*     if(i==I_U_234) FrontEnd.Mass[i]=-4.1503e-6*.9; */
/*     else if(i==I_U_235) FrontEnd.Mass[i]=-1.8489e-4*.9; */
/*     else if(i==I_U_236) FrontEnd.Mass[i]=-1.1186e-4*.9; */
/*     else if(i==I_U_238) FrontEnd.Mass[i]=-2.1567e-2*.9; */
/*     else if(i==I_NP_237) FrontEnd.Mass[i]=-1.4518e-5*.9; */
/*     else if(i==I_PU_238) FrontEnd.Mass[i]=-5.5721e-6*.9; */
/*     else if(i==I_PU_239) FrontEnd.Mass[i]=-1.4086e-4*.9; */
/*     else if(i==I_PU_240) FrontEnd.Mass[i]=-5.5231e-5*.9; */
/*     else if(i==I_PU_241) FrontEnd.Mass[i]=-3.6067e-5*.9; */
/*     else if(i==I_PU_242) FrontEnd.Mass[i]=-1.3920e-5*.9; */
/*     else if(i==I_AM_241) FrontEnd.Mass[i]=-2.8177e-6*.9; */
/*     else if(i==I_AM_243) FrontEnd.Mass[i]=-3.4254e-6*.9; */
// IMF
    if(i==I_NP_237) FrontEnd.Mass[i]=1.13e-1;
    else if(i==I_PU_238) FrontEnd.Mass[i]=3.32e-2;
    else if(i==I_PU_239) FrontEnd.Mass[i]=9.25e-1;
    else if(i==I_PU_240) FrontEnd.Mass[i]=4.12e-1;
    else if(i==I_PU_241) FrontEnd.Mass[i]=6.27e-2;
    else if(i==I_PU_242) FrontEnd.Mass[i]=1.13e-1;
    else if(i==I_AM_241) FrontEnd.Mass[i]=2.05e-1;
    else if(i==I_AM_242) FrontEnd.Mass[i]=1.68e-4;
    else if(i==I_AM_243) FrontEnd.Mass[i]=2.53e-2;
    else if(i==I_CM_242) FrontEnd.Mass[i]=4.06e-7;
    else if(i==I_CM_243) FrontEnd.Mass[i]=5.95e-5;
    else if(i==I_CM_244) FrontEnd.Mass[i]=2.90e-3;
    else if(i==I_CM_245) FrontEnd.Mass[i]=4.08e-4;
    else if(i==I_CM_246) FrontEnd.Mass[i]=5.49e-5;
// BUC IV-A A-0 TheCase 1
/*     if(i==I_U_234) FrontEnd.Mass[i]=-2.7999e-7; */
/*     else if(i==I_U_235) FrontEnd.Mass[i]=-5.8570e-5; */
/*     else if(i==I_U_236) FrontEnd.Mass[i]=0.; */
/*     else if(i==I_U_238) FrontEnd.Mass[i]=-2.3074e-2; */
/*     else if(i==I_NP_237) FrontEnd.Mass[i]=0; */
/*     else if(i==I_PU_238) FrontEnd.Mass[i]=-2.4700e-5; */
/*     else if(i==I_PU_239) FrontEnd.Mass[i]=-8.0623e-4; */
/*     else if(i==I_PU_240) FrontEnd.Mass[i]=-3.1298e-4; */
/*     else if(i==I_PU_241) FrontEnd.Mass[i]=-1.6533e-4; */
/*     else if(i==I_PU_242) FrontEnd.Mass[i]=-5.3981e-5; */
/*     else if(i==I_AM_241) FrontEnd.Mass[i]=0.; */
/*     else if(i==I_AM_243) FrontEnd.Mass[i]=0.; */
/*     else if(i==I_CM_242) FrontEnd.Mass[i]=0.; */
/*     else if(i==I_CM_243) FrontEnd.Mass[i]=0.; */
/*     else if(i==I_CM_244) FrontEnd.Mass[i]=0.; */
/*     else if(i==I_CM_245) FrontEnd.Mass[i]=0.; */
/* // BUC IV-A A-60 TheCase 10 */
/*     if(i==I_U_234) FrontEnd.Mass[i]=-9.1664e-7; */
/*     else if(i==I_U_235) FrontEnd.Mass[i]=-1.9181e-5; */
/*     else if(i==I_U_236) FrontEnd.Mass[i]=-7.5360e-6; */
/*     else if(i==I_U_238) FrontEnd.Mass[i]=-2.1986e-2; */
/*     else if(i==I_NP_237) FrontEnd.Mass[i]=-4.1997e-6; */
/*     else if(i==I_PU_238) FrontEnd.Mass[i]=-2.9509e-5; */
/*     else if(i==I_PU_239) FrontEnd.Mass[i]=-3.6327e-4; */
/*     else if(i==I_PU_240) FrontEnd.Mass[i]=-2.5605e-4; */
/*     else if(i==I_PU_241) FrontEnd.Mass[i]=-1.6525e-4; */
/*     else if(i==I_PU_242) FrontEnd.Mass[i]=-1.1211e-4; */
/*     else if(i==I_AM_241) FrontEnd.Mass[i]=-2.1568e-5; */
/*     else if(i==I_AM_243) FrontEnd.Mass[i]=-3.2566e-5; */
/*     else if(i==I_CM_242) FrontEnd.Mass[i]=-8.4273e-7; */
/*     else if(i==I_CM_243) FrontEnd.Mass[i]=-1.8772e-7; */
/*     else if(i==I_CM_244) FrontEnd.Mass[i]=-2.8565e-5; */
/*     else if(i==I_CM_245) FrontEnd.Mass[i]=-3.9227e-6; */
/* // BUC IV-A B-0 TheCase 20 */
/*     if(i==I_U_234) FrontEnd.Mass[i]=-2.8452e-7; */
/*     else if(i==I_U_235) FrontEnd.Mass[i]=-5.9518e-5; */
/*     else if(i==I_U_236) FrontEnd.Mass[i]=0.; */
/*     else if(i==I_U_238) FrontEnd.Mass[i]=-2.3447e-2; */
/*     else if(i==I_NP_237) FrontEnd.Mass[i]=0; */
/*     else if(i==I_PU_238) FrontEnd.Mass[i]=-4.8972e-7; */
/*     else if(i==I_PU_239) FrontEnd.Mass[i]=-9.1194e-4; */
/*     else if(i==I_PU_240) FrontEnd.Mass[i]=-5.8276e-5; */
/*     else if(i==I_PU_241) FrontEnd.Mass[i]=-3.8689e-6; */
/*     else if(i==I_PU_242) FrontEnd.Mass[i]=-4.8161e-7; */
/*     else if(i==I_AM_241) FrontEnd.Mass[i]=0.; */
/*     else if(i==I_AM_243) FrontEnd.Mass[i]=0.; */
/*     else if(i==I_CM_242) FrontEnd.Mass[i]=0.; */
/*     else if(i==I_CM_243) FrontEnd.Mass[i]=0.; */
/*     else if(i==I_CM_244) FrontEnd.Mass[i]=0.; */
/*     else if(i==I_CM_245) FrontEnd.Mass[i]=0.; */
/* // BUC IV-A B-60 TheCase 29 */
/*     if(i==I_U_234) FrontEnd.Mass[i]=-2.6275e-7; */
/*     else if(i==I_U_235) FrontEnd.Mass[i]=-1.4759e-5; */
/*     else if(i==I_U_236) FrontEnd.Mass[i]=-7.9160e-6; */
/*     else if(i==I_U_238) FrontEnd.Mass[i]=-2.2300e-2; */
/*     else if(i==I_NP_237) FrontEnd.Mass[i]=-4.1080e-6; */
/*     else if(i==I_PU_238) FrontEnd.Mass[i]=-9.6242e-6; */
/*     else if(i==I_PU_239) FrontEnd.Mass[i]=-2.7573e-4; */
/*     else if(i==I_PU_240) FrontEnd.Mass[i]=-1.8198e-4; */
/*     else if(i==I_PU_241) FrontEnd.Mass[i]=-1.1209e-4; */
/*     else if(i==I_PU_242) FrontEnd.Mass[i]=-5.6427e-5; */
/*     else if(i==I_AM_241) FrontEnd.Mass[i]=-1.2034e-5; */
/*     else if(i==I_AM_243) FrontEnd.Mass[i]=-1.5293e-5; */
/*     else if(i==I_CM_242) FrontEnd.Mass[i]=-4.9610e-7; */
/*     else if(i==I_CM_243) FrontEnd.Mass[i]=-9.5010e-8; */
/*     else if(i==I_CM_244) FrontEnd.Mass[i]=-9.2627e-6; */
/*     else if(i==I_CM_245) FrontEnd.Mass[i]=-1.0624e-6; */
/* // BUC IV-A C-0 TheCase 39 */
/*     if(i==I_U_234) FrontEnd.Mass[i]=-2.7318e-7; */
/*     else if(i==I_U_235) FrontEnd.Mass[i]=-5.7145e-5; */
/*     else if(i==I_U_236) FrontEnd.Mass[i]=0.; */
/*     else if(i==I_U_238) FrontEnd.Mass[i]=-2.2513e-2; */
/*     else if(i==I_NP_237) FrontEnd.Mass[i]=0; */
/*     else if(i==I_PU_238) FrontEnd.Mass[i]=-7.8503e-5; */
/*     else if(i==I_PU_239) FrontEnd.Mass[i]=-7.0356e-4; */
/*     else if(i==I_PU_240) FrontEnd.Mass[i]=-5.4493e-4; */
/*     else if(i==I_PU_241) FrontEnd.Mass[i]=-2.3257e-4; */
/*     else if(i==I_PU_242) FrontEnd.Mass[i]=-3.8610e-4; */
/*     else if(i==I_AM_241) FrontEnd.Mass[i]=0.; */
/*     else if(i==I_AM_243) FrontEnd.Mass[i]=0.; */
/*     else if(i==I_CM_242) FrontEnd.Mass[i]=0.; */
/*     else if(i==I_CM_243) FrontEnd.Mass[i]=0.; */
/*     else if(i==I_CM_244) FrontEnd.Mass[i]=0.; */
/*     else if(i==I_CM_245) FrontEnd.Mass[i]=0.; */
/* // BUC IV-A C-60 TheCase 51 */
/*     if(i==I_U_234) FrontEnd.Mass[i]=-2.1766e-6; */
/*     else if(i==I_U_235) FrontEnd.Mass[i]=-2.0961e-5; */
/*     else if(i==I_U_236) FrontEnd.Mass[i]=-7.3406e-6; */
/*     else if(i==I_U_238) FrontEnd.Mass[i]=-2.1428e-2; */
/*     else if(i==I_NP_237) FrontEnd.Mass[i]=-4.3720e-6; */
/*     else if(i==I_PU_238) FrontEnd.Mass[i]=-6.2619e-5; */
/*     else if(i==I_PU_239) FrontEnd.Mass[i]=-4.2049e-4; */
/*     else if(i==I_PU_240) FrontEnd.Mass[i]=-3.4474e-4; */
/*     else if(i==I_PU_241) FrontEnd.Mass[i]=-2.1323e-4; */
/*     else if(i==I_PU_242) FrontEnd.Mass[i]=-3.6386e-4; */
/*     else if(i==I_AM_241) FrontEnd.Mass[i]=-3.0366e-5; */
/*     else if(i==I_AM_243) FrontEnd.Mass[i]=-7.8878e-5; */
/*     else if(i==I_CM_242) FrontEnd.Mass[i]=-1.0431e-6; */
/*     else if(i==I_CM_243) FrontEnd.Mass[i]=-2.4454e-7; */
/*     else if(i==I_CM_244) FrontEnd.Mass[i]=-6.0098e-5; */
/*     else if(i==I_CM_245) FrontEnd.Mass[i]=-9.6412e-6; */
// BUC IV-B A
/*     if(i==I_U_234) FrontEnd.Mass[i]=-2.5952e-7; */
/*     else if(i==I_U_235) FrontEnd.Mass[i]=-5.4287e-5; */
/*     else if(i==I_U_238) FrontEnd.Mass[i]=-2.1387e-2; */
/*     else if(i==I_PU_238) FrontEnd.Mass[i]=-4.6610e-5; */
/*     else if(i==I_PU_239) FrontEnd.Mass[i]=-1.0156e-3; */
/*     else if(i==I_PU_240) FrontEnd.Mass[i]=-4.8255e-4; */
/*     else if(i==I_PU_241) FrontEnd.Mass[i]=-1.7491e-4; */
/*     else if(i==I_PU_242) FrontEnd.Mass[i]=-1.3201e-4; */
// BUC IV-B B
/*     if(i==I_U_234) FrontEnd.Mass[i]=-2.7043e-7; */
/*     else if(i==I_U_235) FrontEnd.Mass[i]=-5.6570e-5; */
/*     else if(i==I_U_238) FrontEnd.Mass[i]=-2.2286e-2; */
/*     else if(i==I_PU_238) FrontEnd.Mass[i]=-4.5941e-7; */
/*     else if(i==I_PU_239) FrontEnd.Mass[i]=-8.5640e-4; */
/*     else if(i==I_PU_240) FrontEnd.Mass[i]=-5.4669e-5; */
/*     else if(i==I_PU_241) FrontEnd.Mass[i]=-2.7221e-6; */
/*     else if(i==I_PU_242) FrontEnd.Mass[i]=-4.5180e-7; */
    else FrontEnd.Mass[i]=0.;
    FrontEnd.MTotal=FrontEnd.MTotal+FrontEnd.Mass[i];
  }
  if(FrontEnd.MTotal < 0.) {
    FrontEnd.MTotal=-FrontEnd.MTotal;
  }
  Grp[reg].Nuclide=Grp[reg].NuclFront;
  for(i=0; i<NACT; i++) {  // assign number density to each actinide based upon mass
     if(FrontEnd.Mass[i] < 0.) {   // specified as number densities already.
        Grp[reg].Nuclide->nd=-FrontEnd.Mass[i];
        FrontEnd.Mass[i]=FrontEnd.MTotal*Grp[reg].Nuclide->nd/rho/N_AVO*(a_matrix+Grp[reg].Nuclide->a);
     } else {
        Grp[reg].Nuclide->nd=rho*N_AVO/(a_matrix+Grp[reg].Nuclide->a)*FrontEnd.Mass[i]/FrontEnd.MTotal;
     }
     if (i==I_U_233 || i==I_U_235 || i==I_PU_239 || i==I_PU_241) ca_nd_guess=ca_nd_guess+Grp[reg].Nuclide->nd/9.;   // control absorber concentration guess
     Grp[reg].Nuclide=Grp[reg].Nuclide->NextNucl;
  }
  ca_nd_guess=0.;
  for(i=0; i<3; i++) { // skip ahead thru lumped fp and Xe&Sm to control absorbers
    Grp[reg].Nuclide=Grp[reg].Nuclide->NextNucl;
  }
  Grp[1-reg].Nuclide=Grp[1-reg].NuclFront;
  while(Grp[reg].Nuclide->ca_nd_factor > 0.) {  // assign ca number densities
    Grp[reg].Nuclide->nd=ca_nd_guess;
    Grp[1-reg].Nuclide->nd=ca_nd_guess;
    Grp[reg].Nuclide=Grp[reg].Nuclide->NextNucl;
    Grp[1-reg].Nuclide=Grp[1-reg].Nuclide->NextNucl;
  }
}

float clad_vol_frac() {

  // determines the volume fraction occupied by the cladding

  switch(Run.Reactor->geomtype) {
  case 0: return((pow(Run.Reactor->r[0],3.)-pow(Run.Reactor->r[0]-Run.Reactor->t_clad,3.))/pow(Run.Reactor->r[0],3.));
    break;
  case 1: return((pow(Run.Reactor->r[0],2.)-pow(Run.Reactor->r[0]-Run.Reactor->t_clad,2.))/pow(Run.Reactor->r[0],2.));
    break;
  case 2: return(Run.Reactor->t_clad/Run.Reactor->r[0]);
    break;
  }
  return(0.);
}

void calc_f_factor() {

  // Calculates the background x-s, then interpolates to find the f-factor, for all nuclides with important=1.  Ignores others.

  int i;
  float sigma_b={0.};  // background x-s for nuclides are stored here

  for(i=0; i<=1; i++) {
    sigma_b=0.;
  // do the sum in the background x-s calculation
    Grp[i].Nuclide=Grp[i].NuclFront;
    while(Grp[i].Nuclide != NULL) {
      sigma_b=sigma_b+(Grp[i].Nuclide->nd)/Grp[i].ndtot*(Grp[i].Nuclide->sigma_tr)*Grp[i].Nuclide->f_calc;
      Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    }
    sigma_b=sigma_b/(1.-Grp[i].Pi);
    Grp[i].Nuclide=Grp[i].NuclFront;
    while(Grp[i].Nuclide != NULL) {
      if (Grp[i].Nuclide->important == 1) {  // check the importance flag; set unimportant f's to 1.
        //if(Grp[i].Nuclide->a==238) printf("Background: %e \n",sigma_b*Grp[i].ndtot/Grp[i].Nuclide->nd-(Grp[i].Nuclide->sigma_tr*Grp[i].Nuclide->f_calc)/(1.-Grp[i].Pi));
        Grp[i].Nuclide->f_calc=interp_f(sigma_b*Grp[i].ndtot/Grp[i].Nuclide->nd-(Grp[i].Nuclide->sigma_tr*Grp[i].Nuclide->f_calc)/(1.-Grp[i].Pi),Grp[i].Nuclide->f_factor);
      }
      else {
        Grp[i].Nuclide->f_calc=1.;
      }
      Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    }
  }
}

float interp_f(float sigma_b, float f_vals[6]) {

  // Given a value for the background cross section, logarithmically interpolates to find the correct f-factor for the given nuclide
  float log_sb;

  log_sb=log10(sigma_b);
  if (log_sb > 10.) return (1.);
  if (log_sb > 5.) return(lininterp(5.,10.,log_sb,f_vals[0],1.));
  if (log_sb > 3.) return(lininterp(3.,5.,log_sb,f_vals[1],f_vals[0]));
  if (log_sb > 2.) return(lininterp(2.,3.,log_sb,f_vals[2],f_vals[1]));
  if (log_sb > 1.) return(lininterp(1.,2.,log_sb,f_vals[3],f_vals[2]));
  if (log_sb > 0.) return(lininterp(0.,1.,log_sb,f_vals[4],f_vals[3]));
  if (log_sb > -1.) return(lininterp(-1.,0.,log_sb,f_vals[5],f_vals[4]));
  return(f_vals[5]);
}

float lininterp(float lb, float ub, float interp, float flb, float fub) {

  // linearly interpolates for a value interp between lb and up, where the function values at lb and ub are flb and fub

  interp=(interp-lb)/(ub-lb);
  return((1.-interp)*flb+interp*fub);
}

void is_important(float sigt) {

  // tests for importance of self-shielding effects based upon 2 criteria: 1) observable self-shielding in the nuclide, 2) infinite dilution criterion.  See section 3.5 of dissertation.

  int i;
  float inf_dil=pow(10.,10.);  // infinite dilution threshold [b/atom]

  for(i=0; i<=1; i++) {
    Grp[i].Nuclide=Grp[i].NuclFront;
    while(Grp[i].Nuclide != NULL) {
      if ((Grp[i].Nuclide->f_factor[5] < 0.9999) && (sigt/(Grp[i].Nuclide->nd) < inf_dil) && (Grp[i].Nuclide->a > 50)) {   // just heavier stuff
        Grp[i].Nuclide->important=1;
      }
      else {
        Grp[i].Nuclide->important=0;
      }
      Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    }
  }
}

float homogenize(float s0, float s1, float vr) {

  // homogenizes a cross section based upon the volume ratio vr=V_0/V_1

  return(s0*(1./(1.+1./vr))+s1*(1./(1.+vr)));
}

void get_sigmas() {

  // derives the transport cross sections in regions 0 and 1

  float S;
  int i=0;

  for(i=0; i<=1; i++) {
    S=0.;
    Grp[i].Nuclide=Grp[i].NuclFront;
    while (Grp[i].Nuclide != NULL) {
      S=S+Grp[i].Nuclide->sigma_tr*Grp[i].Nuclide->nd*Grp[i].Nuclide->f_calc;
      Grp[i].Nuclide=Grp[i].Nuclide->NextNucl;
    }
    Grp[i].Sigma_tr=S;
  }
}

void get_probs(float* pi0, float* pi1) {

  // derives two-region transmission probabilities Pi[0] and Pi[1]

  float p0, t0, t1;

  switch(Run.Reactor->geomtype) {
  case 0:
    t0=get_t0sph(Grp[0].Sigma_tr*Run.Reactor->r[0]);
    p0=reciprocity(Grp[0].Sigma_tr*Run.Reactor->r[0],1./3.,t0);
    t1=get_t1sph(Grp[1].Sigma_tr*Run.Reactor->r[0],Run.Reactor->r[1]/Run.Reactor->r[0]);
    break;
  case 1:
    t0=get_t0cyl(Grp[0].Sigma_tr*Run.Reactor->r[0]);
    p0=reciprocity(Grp[0].Sigma_tr*Run.Reactor->r[0],1./2.,t0);
    t1=get_t1cyl(Grp[1].Sigma_tr*Run.Reactor->r[0]);
    break;
  case 2:
    t0=get_t0slab(Grp[0].Sigma_tr*Run.Reactor->r[0]);
    p0=reciprocity(Grp[0].Sigma_tr*Run.Reactor->r[0],1./2.,t0);
    t1=get_t0slab(Grp[1].Sigma_tr*Run.Reactor->r[1]);
    break;
  default:
    standard_error("Invalid geometry type in derive_Pi");
    break;
  }
  *pi0=get_pi0(t0,p0,t1);
  *pi1=get_pi1(*pi0,Grp[0].Sigma_tr,Grp[1].Sigma_tr,Run.Reactor->volratio);
}

void derive_volratio(float* r, float* volratio, int geomtype) {

  // finds the volume ratio for a given geometry type: 0=sphere, 1=cyl, 2=slab.  For sphere: r[0]=pellet radius, r[1]=fuel volume fraction (initially; this updates it to annular region radius and assigns the correct value to volratio; for cylinder r[0]=pin radius, r[1]=pitch; for slab r[0]=fuel thickness, r[1]=coolant thickness

  switch(geomtype) {
  case 0:
    *(r+1)=pow(*(r)*(*(r))*(*(r))/(*(r+1)),1./3.);
    *volratio=pow(*(r),3.)/(pow(*(r+1),3.)-pow(*(r),3.));
    break;
  case 1:
    *volratio=M_PI*(*(r))*(*(r))/(*(r+1)*(*(r+1))-M_PI*(*(r)*(*r)));
    break;
  case 2:
    *volratio=*(r)/(*(r+1));
    break;
  default:
    standard_error("Invalid geometry type in derive_volratio");
    break;
  }
}

float get_pi0(float t0, float p0, float t1) {

  // Finds the fuel-to-moderator transmission probability Pi_0

  return(p0*(1.-t1)*(1./(1.-t0*t1)));

}

float get_pi1(float pi0, float sigma0, float sigma1, float volratio) {

  // Uses reciprocity to derive the probability Pi_1 from Pi_0; volratio=V_0/V_1

  return(pi0*volratio*sigma0/sigma1);

}

void load_dancoff(float r_to_p) {

  // Loads the dancoff factor data file and performs the appropriate interpolation to derive a vector of from D.F.=f(sigma_a)

  FILE *f;
  float sigma_a,rp,df;  // dummies for reading data
  float interp;  // interpolation factor
  int i;

  float DELTA_RP=0.05;  // mesh spacing in file "lookup"

  f=fopen("lookup","r");
  fscanf(f,"%f %f %f",&sigma_a,&rp,&df);
  fclose(f);  // easiest to close and re-open after getting started
  f=fopen("lookup","r");
  if (r_to_p < rp) standard_error("Unrealistic geometry in load_dancoff");
  while (r_to_p > rp+2.*DELTA_RP) {  // march through the file till we get to the relevant part
    if (rp > .4501) standard_error("Ludicrous geometry in load_dancoff");
    for (i=1; i <= NUM_POINTS_LOOKUP; i++) {  // read these in & throw them away; not relevant to the r/p passed as an input parameter
      fscanf(f,"%f %f %f",&sigma_a,&rp,&df);
    }
    fscanf(f," ");
  }
  for (i=1; i<= NUM_POINTS_LOOKUP; i++) {  // read in the lower of the two arrays to be interpolated on
    fscanf(f,"%f %f %f",&sigma_a,&rp,&Run.dancoff[i-1][0]);
  }
  fscanf(f," ");
  interp=(r_to_p-rp)/DELTA_RP;
  for (i=1; i<= NUM_POINTS_LOOKUP; i++) {  // read in the second array & linearly interpolate
    fscanf(f,"%f %f %f",&Run.dancoff[i-1][1],&rp,&df);
    Run.dancoff[i-1][0]=Run.dancoff[i-1][0]*(1.-interp)+df*interp;
  }
}

void standard_error(char errorstr[]) {

  // Ends program and prints error message to screen

  printf("Runtime Error:\n");
  printf("%s\n",errorstr);
  printf("Exiting to System...\n");
  exit(1);
}

float get_t0slab(float sigma_a) {

  // derives the transmission probability for an infinite slab w/ thickness sigma_a (measured in mean free paths): USES exponential3: works for both T_0 and T_1

  return(1./sigma_a*(0.5-expint(3,sigma_a)));

}

float get_t0sph(float sigma_a) {

  // derives the transmission probability for a sphere of radius sigma_a (measured in mean free paths)

  return(0.5/sigma_a/sigma_a*(1.-exp(-2.*sigma_a)*(1+2.*sigma_a)));

}

float get_t0cyl(float sigma_a) {

  // derives the transmission probability for an infinite cylinder of radius sigma_a (mean free paths): rational approximation of section 3.1

const double C_PARAM=0.3567;  // the C-fitting parameter of section 3.1

 return(1.-2.*sigma_a*(1.+(C_PARAM+2./3.)*sigma_a+8./3.*sigma_a*sigma_a*C_PARAM)/(1.+(C_PARAM+2.)*sigma_a+(2.*C_PARAM+4./3.)*sigma_a*sigma_a+16./3.*sigma_a*sigma_a*sigma_a*C_PARAM));

}

float reciprocity(float sigma_a, float vsr, float t0) {

  // Invokes reciprocity: T_0=1-4*vsr/a*sigma_a*P_0; vsr=1/a*(vol:surface ratio), to find P_0 from T_0

  return((1.-t0)/(4.*sigma_a*vsr));

}

float get_t1sph(float alpha, float beta) {

  // finds the moderator transmission probability (dancoff factor) for a stochastically packed array of spherical fuel elements, e.g. PBMR: rational approx: see section 3.1

  return(1./(1.+4./3.*alpha*(beta*beta*beta-1.)));

}

float get_t1cyl(float sigma_a) {

  // finds the dancoff factor for a square cylindrical lattice using the lookup table generated by the dancoff monte carlo routine

  int i={1};
  float interp;  // for logarithmic interpolation

  if( sigma_a < Run.dancoff[0][1]) return(1.);  // For exceptionally 'sparse' moderators, I suppse
  if (sigma_a > Run.dancoff[NUM_POINTS_LOOKUP-1][1]) return(0.);  // For very dense moderators
  while (sigma_a > Run.dancoff[i][1]) i++;
  interp=(log10(sigma_a)-log10(Run.dancoff[i-1][1]))/(log10(Run.dancoff[i][1])-log10(Run.dancoff[i-1][1]));
  return(Run.dancoff[i-1][0]*(1.-interp)+Run.dancoff[i][0]*interp);
}

void derive_inscatterfine(float* inscf, float ds[][2], int ndown, int ncurrent, float sigma_s, float numden, float mu_cm, float phi, int region) {
                        
  // derive inscattering contributions to source term for the ndown groups lying below group ncurrent: USES derived arguments sigma_s, numden (number density), mu_cm, flux.

  int i={1};
  float tend; // 10^delta
  int bomb=0;

  while(i <= ndown && i+ncurrent < NFAST) {  // check to see that 1) we haven't gone  past the nonzero entries in the ds array, 2) we haven't moved from the fast region to the thermal region
    if(ds[i][0] >=0 ) {
    *(inscf+(i+ncurrent)*2+region)=*(inscf+(i+ncurrent)*2+region)+phi*numden*sigma_s*(ds[i][0]+ds[i][1]*mu_cm);
       if(ds[i][0]+ds[i][1]*mu_cm <= 0) bomb=1;
    }
    else {
      bomb=1;
      printf("Warning! %e %e %e %e\n",(ds[i][0]+mu_cm*ds[i][1]),ds[i][0],ds[i][1],mu_cm);
    }
    i++;
  }
  if (ndown == -1) {   // separate treatment for hydrogen
    tend=pow(10.,DELTAFAST);
    while (i+ncurrent < NFAST) {
      *(inscf+(i+ncurrent)*2+region)=*(inscf+(i+ncurrent)*2+region)+phi*numden*sigma_s/(DELTAL*DELTAFAST)*(pow(tend,-(i+1)))*(tend*tend-2.*tend+1.); // downscattering
      i++;
    }
  }
}

void augment_inscatter_inel(float *insc, float insc_kernel[], int ncurrent, float sigma_i, float numden, float phi, int region, int number_of_groups) {

  // augments inscattering source term with contributions from inelastic scattering; procedure works for fine or coarse structures
  int i={1};

  while(i+ncurrent < number_of_groups) {
    *(insc+(i+ncurrent)*2+region)=*(insc+(i+ncurrent)*2+region)+phi*numden*sigma_i*insc_kernel[i+ncurrent];
    i++;
  }
}

void derive_inscatter(float *insc, float *insc_th, float ds[][2], int ndown, int ncurrent, float sigma_s, float numden, float mu_cm, float phi, int region) {

  // derive inscattering contributions to source term for the ndown groups lying below group ncurrent: USES globals sigma_s, numden (number density), mu_cm, flux: Must be different from inscatterfine since we now want to keep track of THERMAL inscattering

  int i={1};
  int thi={0};  // counter for thermal region
  float tend;  // 10^delta
  float hydtotal={0.};  // running total of hydrogen kernel; used to determine inscattering to the lowest energy thermal group

  while(i <= ndown && i+ncurrent < NFAST/10) {  // check to see that 1) we haven't gone  past the nonzero entries in the ds array, 2) we haven't moved from the fast region to the thermal region
    *(insc+(i+ncurrent)*2+region)=*(insc+(i+ncurrent)*2+region)+phi*numden*sigma_s*(ds[i][0]+ds[i][1]*mu_cm);
    if(ds[i][0]+ds[i][1]*mu_cm <0) printf("Warning.");
    i++;
  }
  while (i+ncurrent < NFAST/10+NTH && i <= ndown) {  // if downscattering extends into the thermal range, add contributions
    *(insc_th+thi*2+region)=*(insc_th+thi*2+region)+phi*numden*sigma_s*(ds[i][0]+ds[i][1]*mu_cm);
    i++;
    if(ds[i][0]+ds[i][1]*mu_cm < 0) printf("%f warning %d %f %f %f \n",ds[i][0]+ds[i][1]*mu_cm,region,ds[i][0],ds[i][1],mu_cm);
    thi++;
  }
  if (ndown == -1) {   // separate treatment for hydrogen
    tend=pow(10.,DELTAFAST*10.); // appropriate for coarse group structure
    hydtotal=hydtotal+(1./tend+DELTAFAST*10.*DELTAL-1.)/(DELTAL*DELTAFAST*10.);
    while (i+ncurrent < NFAST/10) {
      *(insc+(i+ncurrent)*2+region)=*(insc+(i+ncurrent)*2+region)+phi*numden*sigma_s/(DELTAL*DELTAFAST*10.)*(pow(tend,-(i+1)))*(tend*tend-2.*tend+1.); // downscattering
      hydtotal=hydtotal+pow(tend,-(i+1))*(tend*tend-2.*tend+1.)/(DELTAL*DELTAFAST*10.);
      i++;
    }
    tend=pow(10.,DELTATH); // now move on to thermal region
    while (thi < NTH-1) { // downscattering for all but last thermal group
      *(insc_th+thi*2+region)=*(insc_th+thi*2+region)+phi*numden*sigma_s/(DELTAL*DELTATH)*(pow(tend,-(i+(thi)+1)))*(tend*tend-2.*tend+1.); // downscattering
      hydtotal=hydtotal+pow(tend,-(thi+i+1))*(tend*tend-2.*tend+1.)/(DELTAL*DELTATH);
    thi++;
    }
    *(insc_th+thi*2+region)=*(insc_th+thi*2+region)+phi*numden*sigma_s*(1.-hydtotal); // last group gets all inscattering down to zero final energy: necessary for PDF to sum to 1
  }
}

void derive_downscatter(float* ds, float delta, int* ndown, int a, float alpha, float lethrange) {

  // derive downscattering distribution functions; works for both fine and coarse group structures thanks to some rough pointer work: if *anyone* can read this, I'll shoot myself

  int i = {1};   // keep track of group we're scattering to
  //float sum = {0.};  // verification
  float tend;  // 10^delta

  if (a < 1) standard_error("Invalid atomic weight");
  if (a ! =  1 && lethrange > delta) {  // proceed with derivation of matrix if it's a non H-1 nuclide with lethrange > width of lethargy group
    tend = pow(10.,delta);
    *ds = iso_d(tend,delta,alpha);  // selfscattering: isotropic
    *(ds+1) = aniso_d(tend,delta,alpha);   // anisotropic
    while ((i+1)*DELTAL*delta < lethrange) {  // derive inscattering for all groups labelled 'C' in group classification figure
      *(ds+2*i) = iso_c(i,tend,delta,alpha);
      *(ds+2*i+1) = aniso_c(i,tend,delta,alpha);
      i++;
    }
    *(ds+2*(i+1)) = iso_a(i+1,tend,delta,alpha);  // inscattering for group A: iso
    *(ds+2*(i+1)+1) = aniso_a(i+1,tend,delta,alpha);  // aniso
    *(ds+2*i) = iso_b(i,tend,delta,alpha)-*(ds+2*(i+1));  // and for group B: iso
    *(ds+2*i+1) = aniso_b(i,tend,delta,alpha)-*(ds+2*(i+1)+1);  // aniso
    *ndown = i+1;  // the first nd entries in this array are nonzero
  }
  else if(a ! =  1 && lethrange < =  delta) {   // heavy scatterers in coarse group structure have lethrange < delta
    *ndown = 1;  // scattering only to self and adjacent group
    tend = pow(10.,delta);
    *(ds+2) = iso_a(1,tend,delta,alpha);  // inscattering for group A: iso
    *(ds+3) = aniso_a(1,tend,delta,alpha);  // aniso
    *ds = 1.-*(ds+2);   // selfscattering iso  =  1-downscattering
    *(ds+1) = -*(ds+3);  // selfscattering aniso = -downscattering
    //printf("%f %f %f %f\n",*ds,*(ds+1),*(ds+2),*(ds+3));
  }
  else *ndown = -1; // set number of downscattering groups to -1: this indicates H-1, which is treated separately
}

float iso_d(float tend, float delta, float alpha) {

  // isotropic selfscattering probability, tend==10^delta

  return (1./tend+DELTAL*delta-1.)/(1.-alpha)/(DELTAL*delta);  // isotropic selfscattering

}

float aniso_d(float tend, float delta, float alpha) {

  // linearly anisotropic selfscattering probability

  return (tend*tend*(2.*alpha*DELTAL*delta-2.*alpha-1.)+2.*tend*(alpha+1.)-1.)/(1.-alpha)/(DELTAL*delta)/(2.*(alpha-1.))/tend/tend;

}

float iso_c(int n, float tend, float delta, float alpha) {

  // derives the probability for isotropic scattering into group of type C n groups below the source group; tend == 10^delta

  return (1./pow(tend,n+1)*(tend*tend-2.*tend+1))/(1.-alpha)/(DELTAL*delta);
}

float aniso_c(int n, float tend, float delta, float alpha) {

  // probability for anisotropic scattering into group of type C n groups below the source group

  return (2.*(alpha+1)*pow(tend,(n+1))-tend*tend-2.*tend-1.)/(1.-alpha)/(DELTAL*delta)/(2.*(alpha-1.))/pow(tend,2*(n+1))*(tend-1.)*(tend-1.);
}

float iso_a(int n, float tend, float delta, float alpha) {

  // probability for isotropic scattering into group of type A

  return (1./pow(tend,n+1)*(tend+pow(tend,n)*(alpha*DELTAL*delta*(n-1)+alpha*(log(alpha)-1.))))/(1.-alpha)/(DELTAL*delta);
}

float aniso_a(int n, float tend, float delta, float alpha) {

  // probability for linearly anisotropic scattering into group of type A

  return (2.*(alpha+1.)*pow(tend,n+1)+pow(tend,2*n)*(2.*alpha*DELTAL*delta*(n-1)+2.*alpha*log(alpha)-alpha*(alpha+2.))-tend*tend)/(1.-alpha)/(DELTAL*delta)/(2.*(alpha-1.))/pow(tend,2.*n+1);
}

float iso_b(int n, float tend, float delta, float alpha) {

  // probability for isotropic scattering into group of type B (+group of type A; see dissertation)

  return (1./pow(tend,n)*(tend-1-alpha*DELTAL*delta*pow(tend,n)))/(1.-alpha)/(DELTAL*delta);

}

float aniso_b(int n, float tend, float delta, float alpha) {

  // anisotropic scattering into group of type B (+group of type A)

  return(2.*alpha*DELTAL*delta*pow(tend,2.*n)+pow(tend,n)*2.*(alpha+1.)*(1.-tend)+tend*tend-1.)/(1.-alpha)/(DELTAL*delta)/(2.*(-alpha+1.))/pow(tend,2.*n);
}

void get_alpha(float* alpha, float* lethrange, int a) {

  // Derives alpha = ((A-1)/(A+1))^2 and lethrange = ln(1/a)

  *alpha=(a-1.)*(a-1.)/(a+1.)/(a+1.);
  *lethrange=log(1./(*alpha));
}

float convert_mu(float mu_lab, int a) {

  // Converts scattering cosine from lab frame to cm frame; see section 4.1 of dissertation
  float mu_cm_calc;

  mu_cm_calc=(mu_lab-(2./3./a))/(1.-3./5./a/a);
  if (mu_cm_calc < 0.) return(0.);
  else return(mu_cm_calc);
}

void Find_SWU() {

  float swu;  // separative work units; to be solved for

  // initial conditions; will be an input
  Toll.nspecies=3;
  Toll.MEnrich[0]=1000.;
  Toll.MEnrich[1]=0.;
  Toll.MEnrich[2]=0.;
  Toll.XEnrich[0][0]=1.0/100.;
  Toll.XEnrich[0][1]=0.225/100.;
  Toll.XEnrich[0][2]=5./100.;
  Toll.XEnrich[1][0]=.002/100.;
  Toll.XEnrich[2][0]=1.-Toll.XEnrich[0][0]-Toll.XEnrich[1][0];
  Toll.WEnrich[0]=235.;
  Toll.WEnrich[1]=236.;
  Toll.WEnrich[2]=238.;
  Toll.Stages[1]=1.;
  Toll.Stages[2]=1.;
  swu=0.;
  tollenrich(swu);
}

void tollenrich(float swu)
     // Given feed mass and composition, computes product and tails mass and composition; uses De La Garza's algorithm

{
  float tol={.00001};  // tolerance for minimization routine
  float MKeyGuess={236.1};  // initial guess for M_Key; keep it between 235 and 236.5 for the U enrichment calculation

 // Compute product and tails masses from mass balance
  Toll.MEnrich[1]=Toll.MEnrich[0]*(Toll.XEnrich[0][0]-Toll.XEnrich[0][2])/(Toll.XEnrich[0][1]-Toll.XEnrich[0][2]);
  Toll.MEnrich[2]=Toll.MEnrich[0]-Toll.MEnrich[1];

  // calculate swu by cascade total flow minimization
  swu=Toll.MEnrich[0]*brent(Toll.WEnrich[0],MKeyGuess,(Toll.WEnrich[0]+Toll.WEnrich[2])/2.,flowfunc,tol,&Toll.MKey)*(Toll.WEnrich[Toll.nspecies-1]-Toll.WEnrich[0])*(Toll.WEnrich[Toll.nspecies-1]-Toll.WEnrich[0])/4.;

  // calculate unknown mol fractions from total flow
  calc_x();

  printf("Feed:    %f %f %f %f\n",Toll.MEnrich[0],Toll.XEnrich[0][0]*100.,Toll.XEnrich[1][0]*100.,Toll.XEnrich[2][0]*100.);
  printf("Tails:   %f %f %f %f\n",Toll.MEnrich[1],Toll.XEnrich[0][1]*100.,Toll.XEnrich[1][1]*100.,Toll.XEnrich[2][1]*100.);
  printf("Product: %f %f %f %f\n",Toll.MEnrich[2],Toll.XEnrich[0][2]*100.,Toll.XEnrich[1][2]*100.,Toll.XEnrich[2][2]*100.);
  printf("SWU: %f M_Key %f Stages %f %f\n",swu,Toll.MKey,Toll.Stages[1],Toll.Stages[2]);
  swu=-(1.-2.*Toll.XEnrich[0][0])*log((1.-Toll.XEnrich[0][0])/Toll.XEnrich[0][0])*Toll.MEnrich[0]+(1.-2.*Toll.XEnrich[0][1])*log((1.-Toll.XEnrich[0][1])/Toll.XEnrich[0][1])*Toll.MEnrich[1]+(1.-2.*Toll.XEnrich[0][2])*log((1.-Toll.XEnrich[0][2])/Toll.XEnrich[0][2])*Toll.MEnrich[2];
  printf("w/o 236: Tails: %f Prod: %f SWU: %f\n",Toll.MEnrich[1],Toll.MEnrich[2],swu);
}

void calc_x()

     // Calculate the unknown enrichments

{
  int i;
  float AI[3],delta[3]; // bookkeeping variables

  for(i=0; i <= Toll.nspecies-1; i++) {   // Compute the A's, 2k-1's
    delta[i]=Toll.MKey-Toll.WEnrich[i];
    AI[i]=calc_a(Toll.Stages,delta[i]);
  }
  // Compute an unknown enrichment directly based upon number of stripper and enricher stages for the given key mass; this is currently implemented for 3 species only
  Toll.XEnrich[1][1]=Toll.XEnrich[1][0]*(1.-AI[1])/(Toll.XEnrich[0][0]*(1.-AI[0])+Toll.XEnrich[1][0]*(1.-AI[1])+Toll.XEnrich[2][0]*(1.-AI[2]));

  // Compute the remaining 4 enrichments by the mass balances
  Toll.XEnrich[2][1]=(Toll.MEnrich[0]*(Toll.XEnrich[1][0]+Toll.XEnrich[2][0])-Toll.MEnrich[2]*(1.-Toll.XEnrich[0][2]))/Toll.MEnrich[1]-Toll.XEnrich[1][1];
  Toll.XEnrich[1][2]=(Toll.MEnrich[0]*Toll.XEnrich[1][0]-Toll.MEnrich[1]*Toll.XEnrich[1][1])/Toll.MEnrich[2];
  Toll.XEnrich[2][2]=1.-Toll.XEnrich[0][2]-Toll.XEnrich[1][2];
}

float calc_a(float x[], float delta)

     // calculate the A_i of De La Garza

{
  return((1.-exp(x[1]*delta))/(exp(-x[2]*delta)-exp(x[1]*delta)));
}

void efunc(int n, float x[], float fvec[])

{
     // Evaluates the two functions, fvec[1] & [2], to be zeroed for the stripper and enricher variables S_s, S_e (Toll.Stages[1] & Toll.Stages[2]; here: x[1] & x[2])

     //  fvec[1] and fvec[2] are functions to be zeroed
     float denom; // dummy for sum computation
     float AI[3]; // The A_i from De La Garza
     float delta[3];  // Mass difference from key weight; see dissertation
     int i;

  denom=0.;
  for(i=0; i <= Toll.nspecies-1; i++) {   // Compute the A's
    delta[i]=Toll.MKey-Toll.WEnrich[i];
    AI[i]=calc_a(x,delta[i]);
  }
  fvec[2]=Toll.XEnrich[0][0]*AI[0];
  for(i=0; i<= Toll.nspecies-1; i++) {
    denom=denom+Toll.XEnrich[i][0]*AI[i];
  }
  fvec[2]=fvec[2]/denom-Toll.XEnrich[0][2];
  denom=0.;
  fvec[1]=Toll.XEnrich[0][0]*(1.-AI[0]);
  for(i=0; i <= Toll.nspecies-1; i++) {
    denom=denom+Toll.XEnrich[i][0]*(1.-AI[i]);
  }
  fvec[1]=fvec[1]/denom-Toll.XEnrich[0][1];
}

float flowfunc(float m)

     // Evaluates cascade total flow for a given key mass; in the process determines number of stripper and enricher steps

{
  int failflag;  // returns 0 on success of broydn
  int i;
  float flowsum={0.},AI[3],delta[3]; // bookkeeping variables

  Toll.MKey=m;   // Update key mass guess
  broydn(Toll.Stages,2,&failflag,efunc);  // solve for S_s and S_e
  for(i=0; i <= Toll.nspecies-1; i++) {   // Compute the A's, 2k-1's
    delta[i]=m-Toll.WEnrich[i];
    AI[i]=calc_a(Toll.Stages,delta[i]);
  }
  for(i=0; i<= Toll.nspecies-1; i++) {
    flowsum=flowsum+(AI[i]*Toll.Stages[2]-(1.-AI[i])*Toll.Stages[1])/delta[i]*Toll.XEnrich[i][0];
  }
  return(flowsum);
}

void economic_module() {

  // Some initialization
  Fuel.Cycles=Fuel.CycFront;
  Fuel.Costs=Fuel.CostFront;
  Fuel.ElecGen=0.;
  Fuel.DRate=.05;
  // These are treated as constants but must be derived here due to C requirements for array dimension specification
  NFLOWS=sizeof(Fuel.Cycles->MassFlow)/sizeof(Fuel.Cycles->MassFlow[0]);
  NCATS=sizeof(Fuel.Costs->MassType)/sizeof(Fuel.Costs->MassType[0]);
  Populate();
  LoadCyc();
  Fuel.Costs=Fuel.CostFront;
  Fuel.Cycles=Fuel.CycFront;
  // Process base case cost data for comparison
  Fuel.ORCAIndex=0;
  ProcessCycle();
  DeriveCost();
  // Clean up; Proceed to test case.  Recall that the direct disposal base case is always the first thing pointed to by Fuel.Cycle.  There's no reason the base case must be DD; if I see a benefit, I'll allow repro. cases to be compared
  WipeCosts();
  Fuel.Cycles=Fuel.Cycles->NextCyc;
  Fuel.ORCAIndex=1;  // Switch this flag to its test case position
  Fuel.ElecGen=0.;
  while (Fuel.Cycles != NULL) {
    // Go through all the cycles to derive costs; recall that the first cycle is always the reference cycle
    ProcessCycle();
    Fuel.Cycles=Fuel.Cycles->NextCyc;
  }
  DeriveCost();
  DeriveCostDiff();
}

//******Linked List maintenance utilities follow********//

COSTSTR *Create_costnode(COSTSTR *cost) {

  // Assign relevant values to this member of the cost list

     COSTSTR *result = (COSTSTR *) malloc(sizeof(COSTSTR));
     result->UnitCost = cost->UnitCost;
     result->Var = cost->Var;
     result->LagTime = cost->LagTime;
     result->Duration = cost->Duration;
     result->UnitVar[0]=cost->UnitVar[0];
     result->UnitVar[1]=cost->UnitVar[1];
     result->CostType=cost->CostType;
     result->MassType[0]=cost->MassType[0];
     result->MassType[1]=cost->MassType[1];
     result->MassType[2]=cost->MassType[2];
     strcpy(result->Name,cost->Name);
     result->NextCost = NULL;
     return result;
 }

CYCSTR *Create_cycnode(CYCSTR *cyc) {

  // Assign relevant values to this member of the cycle list

     CYCSTR *result = (CYCSTR *) malloc(sizeof(CYCSTR));
     int i;

     result->RefTime = cyc->RefTime;
     result->Category = cyc->Category;
     result->TRes = cyc->TRes;
     for (i=0; i<=NFLOWS-1; i++) {
       result->MassFlow[i] = cyc->MassFlow[i];
     }
     result->NextCyc = NULL;
     return result;
 }

void Insert_costnode(COSTSTR *place, COSTSTR *cost) {

  // Create a link to the next member of the cost list; call the routine that assigns values

  place->NextCost = Create_costnode(cost);
}

void Insert_cycnode(CYCSTR *place, CYCSTR *cyc) {

  // Create a link to the next member of the cycle list; call the routine that assigns values

  place->NextCyc = Create_cycnode(cyc);
}

void Create_empty_list() {

  // Initialize the linked lists to point to nothing

     Fuel.CostFront=Fuel.Costs=NULL;
     Fuel.CycFront=Fuel.Cycles=NULL;
}

void Link_costlist(COSTSTR *firstcost) {

  // Link the first element in the cost list

  Fuel.CostFront=Create_costnode(firstcost);
  Fuel.Costs=Fuel.CostFront;
}

void Link_cyclist(CYCSTR *firstcyc) {

  // Link the first element in the cycle list

  Fuel.CycFront=Create_cycnode(firstcyc);
  Fuel.Cycles=Fuel.CycFront;
}


void Populate()

     // Read input data from the file "costdata" (*must* be located in same directory as executable) and create the linked list of cost data

{
  FILE *f;
  COSTSTR Cost;  // Dummy that holds data as it's read from file
  char line_read[30];
  int i,j;

  f=fopen("costdata","r");
  Create_empty_list();
  fgets(line_read,30,f);
  j=0;
  while(line_read[0] != '#') {  // The character '#' MUST terminate the input
  // Go through some contortions to read a string that may contain spaces...
    i=0;
    while (line_read[i] != '\0') {
      Cost.Name[i]=line_read[i];
      i++;
    }
    Cost.Name[i-1]='\0';
    // Read numerical values according to this format:
    // UnitCost SPC %Dev SPC Type SPC LagTime SPC Duration(if relevant) SPC UnitVar[0](if relevant) SPC UnitVar[1](if relevant) SPC MassType[0] SPC MassType[1] SPC MassType[2] CR
    fscanf(f,"%f %f %d %f",&Cost.UnitCost,&Cost.Var,&Cost.CostType,&Cost.LagTime);
    Cost.Var=Cost.Var*Cost.Var*Cost.UnitCost*Cost.UnitCost/10000.; // Convert standard dev (%) to variance
    switch (Cost.CostType) {
      case 1: fscanf(f,"%f",&Cost.Duration);
        break;
      case 2: fscanf(f,"%f",&Cost.UnitVar[0]);
        break;
      case 3: fscanf(f,"%f %f",&Cost.Duration,&Cost.UnitVar[0]);
        break;
      case 4: fscanf(f,"%f %f",&Cost.Duration,&Cost.UnitVar[1]);
        break;
      case 5: fscanf(f,"%f %f %f",&Cost.Duration,&Cost.UnitVar[0],&Cost.UnitVar[1]);
        break;
    }
    for (i=0; i<=NCATS-1; i++) {
      fscanf(f,"%d",&Cost.MassType[i]);
    }
    fscanf(f,"\n");
    // Assemble the list: the very first list element must be linked to the Fuel struct; the others are linked sequentially
    if(j==0) {
      Link_costlist(&Cost);
      j=1;
    }
    else {
      Insert_costnode(Fuel.Costs,&Cost);
      Fuel.Costs=Fuel.Costs->NextCost;
    }
    fgets(line_read,30,f);
  }
}

void LoadCyc()

     // Temporary routine to allow me to debug; these will all be material balance outputs

{
  CYCSTR Cyc;
  Cyc.RefTime=0.;
  Cyc.Category=0;
  Cyc.TRes=3.756;
  Cyc.MassFlow[0]=14.535;
  Cyc.MassFlow[1]=8091.;
  Cyc.MassFlow[2]=8091.*.995;
  Cyc.MassFlow[3]=6442.;
  Cyc.MassFlow[4]=1000.;
  Cyc.MassFlow[5]=1000.;
  Cyc.MassFlow[6]=0.;
  Cyc.MassFlow[7]=0.;
  Cyc.MassFlow[8]=0.;
  Link_cyclist(&Cyc);
  // Now we try the 1,0 single Pu cycle: first the UO2
  Cyc.RefTime=0.;
  Cyc.Category=0;
  Cyc.TRes=3.756;
  Cyc.MassFlow[0]=14.535;
  Cyc.MassFlow[1]=8091.;
  Cyc.MassFlow[2]=8091.*.995;
  Cyc.MassFlow[3]=6442.;
  Cyc.MassFlow[4]=1000.;
  Cyc.MassFlow[5]=0.;
  Cyc.MassFlow[6]=1000.;
  Cyc.MassFlow[7]=1000.;
  Cyc.MassFlow[8]=0.;
  Insert_cycnode(Fuel.Cycles,&Cyc);
  Fuel.Cycles=Fuel.Cycles->NextCyc;
  Cyc.RefTime=9.+Fuel.Cycles->TRes;
  Cyc.Category=1;
  Cyc.TRes=3.756;
  Cyc.MassFlow[0]=14.535;
  Cyc.MassFlow[1]=0.;
  Cyc.MassFlow[2]=0.;
  Cyc.MassFlow[3]=0.;
  Cyc.MassFlow[4]=162.;
  Cyc.MassFlow[5]=162.;
  Cyc.MassFlow[6]=0.;
  Cyc.MassFlow[7]=0.;
  Cyc.MassFlow[8]=0.;
  Insert_cycnode(Fuel.Cycles,&Cyc);
  Fuel.Cycles=Fuel.Cycles->NextCyc;
}

void DeriveCost()

     // Derive the costs in mills/kWh for the scenario being considered

{
  Fuel.Costs=Fuel.CostFront;
  while (Fuel.Costs != NULL) {
    Fuel.Costs->CostSum=1000.*Fuel.Costs->CostSum/Fuel.ElecGen;  // converting to mills/kWh
    Fuel.Orca[Fuel.ORCAIndex].TotCost=Fuel.Orca[Fuel.ORCAIndex].TotCost+Fuel.Costs->CostSum;
    Fuel.Orca[Fuel.ORCAIndex].Variance=Fuel.Orca[Fuel.ORCAIndex].Variance+(Fuel.Costs->CostSum/Fuel.Costs->UnitCost)*(Fuel.Costs->CostSum/Fuel.Costs->UnitCost)*Fuel.Costs->Var;
    printf("%30s %f\n",Fuel.Costs->Name,Fuel.Costs->CostSum);
    Fuel.Costs=Fuel.Costs->NextCost;
  }
  printf("Total: %f StDev: %f\n",Fuel.Orca[Fuel.ORCAIndex].TotCost,sqrt(Fuel.Orca[Fuel.ORCAIndex].Variance));
}

void DeriveCostDiff()

// Derives the difference in cost between base and test cases and its variance

{
  Fuel.CostDiffVar=0.;
  Fuel.Costs=Fuel.CostFront;
  while (Fuel.Costs != NULL) {
    Fuel.CostDiffVar=Fuel.CostDiffVar-Fuel.Costs->CostSum/Fuel.Costs->UnitCost*Fuel.Costs->PartialCost*Fuel.Costs->Var; // This is the summation of covariances
    Fuel.Costs=Fuel.Costs->NextCost;
  }
  Fuel.CostDiff=Fuel.Orca[1].TotCost-Fuel.Orca[0].TotCost;
  Fuel.CostDiffVar=Fuel.CostDiffVar+Fuel.Orca[0].Variance+Fuel.Orca[1].Variance;
  printf("Difference (Test - Base): %f StDev: %f\n",Fuel.CostDiff,sqrt(Fuel.CostDiffVar));
}


void WipeCosts()

     // This is run after base case execution; Stores CostSum/UnitCost in the PartialCost variable (for later use for derivation of variance of the cost difference), then cleans up the Fuel.Costs->CostSum variables

{
  Fuel.Costs=Fuel.CostFront;
  while(Fuel.Costs != NULL) {
    Fuel.Costs->PartialCost=Fuel.Costs->CostSum/Fuel.Costs->UnitCost;
    Fuel.Costs->CostSum=0.;
    Fuel.Costs=Fuel.Costs->NextCost;
  }
}

void ProcessCycle()

     // Processes a single cycle

{
  float mass; // the mass relevant to the unit cost being considered
  float PVF;  // present value factor
  float TBegin;  // Time at which cost is initially incurred; used to handle the issue that LagTime may be a lead from fuel *charge* or lag from fuel *discharge*

  Fuel.Costs=Fuel.CostFront;
  while (Fuel.Costs != NULL) {
    if (Fuel.Costs->MassType[Fuel.Cycles->Category] > 0) {
      // Is this cost relevant to the cycle being studied?
      mass=Fuel.Cycles->MassFlow[Fuel.Costs->MassType[Fuel.Cycles->Category]];
      if (Fuel.Costs->LagTime > 0.) {
        TBegin=Fuel.Costs->LagTime+Fuel.Cycles->TRes;
      }
      else TBegin=Fuel.Costs->LagTime;
      switch(Fuel.Costs->CostType) {
        // Derive the appropriate present value factor for the cost type; for distributed costs multiply it by the time so that we can simply take (cost=PVF*Unit_Cost)
        case 0: PVF=OneTimeCost(Fuel.DRate,TBegin+Fuel.Cycles->RefTime);
          break;
        case 1: PVF=DistribCost(Fuel.DRate,TBegin+Fuel.Cycles->RefTime,Fuel.Costs->Duration);
          PVF=PVF*Fuel.Costs->Duration;
          break;
        case 2: PVF=OTCostDPayment(Fuel.DRate,TBegin+Fuel.Cycles->RefTime,Fuel.Cycles->RefTime,Fuel.Cycles->TRes,Fuel.Costs->UnitVar[0]);
          break;
        case 3: PVF=DCostDPayment(Fuel.DRate,TBegin+Fuel.Cycles->RefTime,Fuel.Costs->Duration,Fuel.Cycles->RefTime,Fuel.Cycles->TRes,Fuel.Costs->UnitVar[0]);
          PVF=PVF*Fuel.Costs->Duration;
          break;
        case 4: PVF=EscDistCost(Fuel.DRate,TBegin+Fuel.Cycles->RefTime,Fuel.Costs->Duration,Fuel.Costs->UnitVar[1]);
          PVF=PVF/Fuel.Costs->UnitVar[1]*(exp(-Fuel.Costs->UnitVar[1]*Fuel.Costs->Duration)-1.); // This takes into account the escalation of the unit cost
          break;
        case 5: PVF=EscDistCostDP(Fuel.DRate,TBegin+Fuel.Cycles->RefTime,Fuel.Costs->Duration,Fuel.Cycles->RefTime,Fuel.Cycles->TRes,Fuel.Costs->UnitVar[0],Fuel.Costs->UnitVar[1]);
          PVF=PVF/Fuel.Costs->UnitVar[1]*(exp(-Fuel.Costs->UnitVar[1]*Fuel.Costs->Duration)-1.);
          break;
        default:  printf("Error in Input Deck: Invalid Cost Category\n");
          break;
      }
      Fuel.Costs->CostSum=Fuel.Costs->CostSum+mass*Fuel.Costs->UnitCost*PVF; // Add the cost to the running total for this cost category
    }
    Fuel.Costs=Fuel.Costs->NextCost;
  }
  // Determine discounted revenue (units of kWh) for this cycle
  PVF=DistribCost(Fuel.DRate,Fuel.Cycles->RefTime,Fuel.Cycles->TRes);
  Fuel.ElecGen=Fuel.ElecGen+PVF*Fuel.Cycles->MassFlow[0]*Fuel.Cycles->MassFlow[4]*24000.;  // discounted generation = PVF*burnup*efficiency*mass*1000*24
}

float OneTimeCost(float R, float t1)

     // Determine the PVF for a one-time cost at time t1

{
  return(exp(-R*t1));
}

float DistribCost(float R, float t1, float T)

     // Determine the PVF for a distributed cost beginning at t1, duration T

{
  return(exp(-R*t1)*(1.-exp(-R*T))/(R*T));
}

float OTCostDPayment(float R, float t1, float t0, float D, float r)

     // Determine the PVF for a one-time cost at time t1, paid for via a fund invested beginning at t0 for a period D earning at a rate r

{
  return(OneTimeCost(R,t0)*(1.-exp(-R*D))/R*(r*exp(-r*(t1-t0)))/(1.-exp(-r*D)));
}

float DCostDPayment(float R, float t1, float T, float t0, float D, float r)

     // Determine the PVF for a distributed cost beginning at time t1 for a period T, paid for via a fund invested beginning at t0 for a period D earning at a rate r

{
  return(OTCostDPayment(R,t1,t0,D,r)*(1.-exp(-r*T))/(r*T));
}

float EscDistCost(float R, float t1, float T, float s)

     // Determine the PVF for an escalating distributed cost beginning at t1, duration T, escalating at rate s

{
  return(exp(-R*t1)*s/(s-R)*(exp((s-R)*T)-1.)/(exp(s*T)-1.));
}

float EscDistCostDP(float R, float t1, float T, float t0, float D, float r, float s)

     // Determine the PDF for an escalating distributed cost beginning at t1, duration T, escalating at s, paid for via a find invested beginning at t0 for a period D earning at a rate r

{
  return(OTCostDPayment(R,t1,t0,D,r)*s/(s-r)*(exp((s-r)*T)-1.)/(exp(s*T)-1.));
}

/* NUMERICAL RECPIES SUBROUTINES FOLLOW
   Cit. Teukoulsky, Press, Flannery, Vettering, "Numerical Recipes in C," 2nd. Ed.
   */

#define NR_END 1
#define FREE_ARG char*
#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))
static float sqrarg;
#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg)

static double dsqrarg;
#define DSQR(a) ((dsqrarg=(a)) == 0.0 ? 0.0 : dsqrarg*dsqrarg)

static double dmaxarg1,dmaxarg2;
#define DMAX(a,b) (dmaxarg1=(a),dmaxarg2=(b),(dmaxarg1) > (dmaxarg2) ?\
        (dmaxarg1) : (dmaxarg2))

static double dminarg1,dminarg2;
#define DMIN(a,b) (dminarg1=(a),dminarg2=(b),(dminarg1) < (dminarg2) ?\
        (dminarg1) : (dminarg2))

static float maxarg1,maxarg2;
#define FMAX(a,b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ?\
        (maxarg1) : (maxarg2))

static float minarg1,minarg2;
#define FMIN(a,b) (minarg1=(a),minarg2=(b),(minarg1) < (minarg2) ?\
        (minarg1) : (minarg2))

static long lmaxarg1,lmaxarg2;
#define LMAX(a,b) (lmaxarg1=(a),lmaxarg2=(b),(lmaxarg1) > (lmaxarg2) ?\
        (lmaxarg1) : (lmaxarg2))

static long lminarg1,lminarg2;
#define LMIN(a,b) (lminarg1=(a),lminarg2=(b),(lminarg1) < (lminarg2) ?\
        (lminarg1) : (lminarg2))

static int imaxarg1,imaxarg2;
#define IMAX(a,b) (imaxarg1=(a),imaxarg2=(b),(imaxarg1) > (imaxarg2) ?\
        (imaxarg1) : (imaxarg2))

static int iminarg1,iminarg2;
#define IMIN(a,b) (iminarg1=(a),iminarg2=(b),(iminarg1) < (iminarg2) ?\
        (iminarg1) : (iminarg2))

void nrerror(char error_text[])
/* Numerical Recipes standard error handler */
{
        printf("Numerical Recipes run-time error...\n");
        printf("%s\n",error_text);
        printf("...now exiting to system...\n");
        exit(1);
}

float *vector(long nl, long nh)
/* allocate a float vector with subscript range v[nl..nh] */
{
        float *v;

        v=(float *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(float)));
        if (!v) nrerror("allocation failure in vector()");
        return v-nl+NR_END;
}

int *ivector(long nl, long nh)
/* allocate an int vector with subscript range v[nl..nh] */
{
        int *v;

        v=(int *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(int)));
        if (!v) nrerror("allocation failure in ivector()");
        return v-nl+NR_END;
}

unsigned char *cvector(long nl, long nh)
/* allocate an unsigned char vector with subscript range v[nl..nh] */
{
        unsigned char *v;

        v=(unsigned char *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(unsigned char)));
        if (!v) nrerror("allocation failure in cvector()");
        return v-nl+NR_END;
}

unsigned long *lvector(long nl, long nh)
/* allocate an unsigned long vector with subscript range v[nl..nh] */
{
        unsigned long *v;

        v=(unsigned long *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(long)));
        if (!v) nrerror("allocation failure in lvector()");
        return v-nl+NR_END;
}

double *dvector(long nl, long nh)
/* allocate a double vector with subscript range v[nl..nh] */
{
        double *v;

        v=(double *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(double)));
        if (!v) nrerror("allocation failure in dvector()");
        return v-nl+NR_END;
}

float **matrix(long nrl, long nrh, long ncl, long nch)
/* allocate a float matrix with subscript range m[nrl..nrh][ncl..nch] */
{
        long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
        float **m;

        /* allocate pointers to rows */
        m=(float **) malloc((size_t)((nrow+NR_END)*sizeof(float*)));
        if (!m) nrerror("allocation failure 1 in matrix()");
        m += NR_END;
        m -= nrl;

        /* allocate rows and set pointers to them */
        m[nrl]=(float *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(float)));
        if (!m[nrl]) nrerror("allocation failure 2 in matrix()");
        m[nrl] += NR_END;
        m[nrl] -= ncl;

        for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

        /* return pointer to array of pointers to rows */
        return m;
}


// Jan 19 Andy Osborne - free stuff
//void freeCmatrix(char** m, long nrl, long nrh, long ncl, long nch)
void freeCmatrix(char** m, long nrl)
{
    //long nrow = nrh-nrl+1;
    free(m[nrl] - 1);
    free(m-1);

}


char **cmatrix(long nrl, long nrh, long ncl, long nch)
/* allocate a char matrix with subscript range m[nrl..nrh][ncl..nch] */
{
        long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
        char **m;

        /* allocate pointers to rows */
        m=(char **) malloc((size_t)((nrow+NR_END)*sizeof(char*)));
        if (!m) nrerror("allocation failure 1 in matrix()");
        m += NR_END;
        m -= nrl;

        /* allocate rows and set pointers to them */
        m[nrl]=(char *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(char)));
        if (!m[nrl]) nrerror("allocation failure 2 in matrix()");
        m[nrl] += NR_END;
        m[nrl] -= ncl;

        for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

        /* return pointer to array of pointers to rows */
        return m;
}

int **imatrix(long nrl, long nrh, long ncl, long nch)
/* allocate a int matrix with subscript range m[nrl..nrh][ncl..nch] */
{
        long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
        int **m;

        /* allocate pointers to rows */
        m=(int **) malloc((size_t)((nrow+NR_END)*sizeof(int*)));
        if (!m) nrerror("allocation failure 1 in matrix()");
        m += NR_END;
        m -= nrl;


        /* allocate rows and set pointers to them */
        m[nrl]=(int *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(int)));
        if (!m[nrl]) nrerror("allocation failure 2 in matrix()");
        m[nrl] += NR_END;
        m[nrl] -= ncl;

        for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

        /* return pointer to array of pointers to rows */
        return m;
}

float **submatrix(float **a, long oldrl, long oldrh, long oldcl, long oldch,
        long newrl, long newcl)
/* point a submatrix [newrl..][newcl..] to a[oldrl..oldrh][oldcl..oldch] */
{
        long i,j,nrow=oldrh-oldrl+1,ncol=oldcl-newcl;
        float **m;

        /* allocate array of pointers to rows */
        m=(float **) malloc((size_t) ((nrow+NR_END)*sizeof(float*)));
        if (!m) nrerror("allocation failure in submatrix()");
        m += NR_END;
        m -= newrl;

        /* set pointers to rows */
        for(i=oldrl,j=newrl;i<=oldrh;i++,j++) m[j]=a[i]+ncol;

        /* return pointer to array of pointers to rows */
        return m;
}

float **convert_matrix(float *a, long nrl, long nrh, long ncl, long nch)
/* allocate a float matrix m[nrl..nrh][ncl..nch] that points to the matrix
declared in the standard C manner as a[nrow][ncol], where nrow=nrh-nrl+1
and ncol=nch-ncl+1. The routine should be called with the address
&a[0][0] as the first argument. */
{
        long i,j,nrow=nrh-nrl+1,ncol=nch-ncl+1;
        float **m;

        /* allocate pointers to rows */
        m=(float **) malloc((size_t) ((nrow+NR_END)*sizeof(float*)));
        if (!m) nrerror("allocation failure in convert_matrix()");
        m += NR_END;
        m -= nrl;

        /* set pointers to rows */
        m[nrl]=a-ncl;
        for(i=1,j=nrl+1;i<nrow;i++,j++) m[j]=m[j-1]+ncol;
        /* return pointer to array of pointers to rows */
        return m;
}

float ***f3tensor(long nrl, long nrh, long ncl, long nch, long ndl, long ndh)
/* allocate a float 3tensor with range t[nrl..nrh][ncl..nch][ndl..ndh] */
{
        long i,j,nrow=nrh-nrl+1,ncol=nch-ncl+1,ndep=ndh-ndl+1;
        float ***t;

        /* allocate pointers to pointers to rows */
        t=(float ***) malloc((size_t)((nrow+NR_END)*sizeof(float**)));
        if (!t) nrerror("allocation failure 1 in f3tensor()");
        t += NR_END;
        t -= nrl;

        /* allocate pointers to rows and set pointers to them */
        t[nrl]=(float **) malloc((size_t)((nrow*ncol+NR_END)*sizeof(float*)));
        if (!t[nrl]) nrerror("allocation failure 2 in f3tensor()");
        t[nrl] += NR_END;
        t[nrl] -= ncl;

        /* allocate rows and set pointers to them */
        t[nrl][ncl]=(float *) malloc((size_t)((nrow*ncol*ndep+NR_END)*sizeof(float)));
        if (!t[nrl][ncl]) nrerror("allocation failure 3 in f3tensor()");
        t[nrl][ncl] += NR_END;
        t[nrl][ncl] -= ndl;

        for(j=ncl+1;j<=nch;j++) t[nrl][j]=t[nrl][j-1]+ndep;
        for(i=nrl+1;i<=nrh;i++) {
                t[i]=t[i-1]+ncol;
                t[i][ncl]=t[i-1][ncl]+ncol*ndep;
                for(j=ncl+1;j<=nch;j++) t[i][j]=t[i][j-1]+ndep;
        }

        /* return pointer to array of pointers to rows */
        return t;
}

void free_vector(float *v, long nl, long nh)
/* free a float vector allocated with vector() */
{
        free((FREE_ARG) (v+nl-NR_END));
}

void free_ivector(int *v, long nl, long nh)
/* free an int vector allocated with ivector() */
{
        free((FREE_ARG) (v+nl-NR_END));
}

void free_cvector(unsigned char *v, long nl, long nh)
/* free an unsigned char vector allocated with cvector() */
{
        free((FREE_ARG) (v+nl-NR_END));
}

void free_lvector(unsigned long *v, long nl, long nh)
/* free an unsigned long vector allocated with lvector() */
{
        free((FREE_ARG) (v+nl-NR_END));
}

void free_dvector(double *v, long nl, long nh)
/* free a double vector allocated with dvector() */
{
        free((FREE_ARG) (v+nl-NR_END));
}

void free_matrix(float **m, long nrl, long nrh, long ncl, long nch)
/* free a float matrix allocated by matrix() */
{
        free((FREE_ARG) (m[nrl]+ncl-NR_END));
        free((FREE_ARG) (m+nrl-NR_END));
}

void free_dmatrix(double **m, long nrl, long nrh, long ncl, long nch)
/* free a double matrix allocated by dmatrix() */
{
        free((FREE_ARG) (m[nrl]+ncl-NR_END));
        free((FREE_ARG) (m+nrl-NR_END));
}

void free_imatrix(int **m, long nrl, long nrh, long ncl, long nch)
/* free an int matrix allocated by imatrix() */
{
        free((FREE_ARG) (m[nrl]+ncl-NR_END));
        free((FREE_ARG) (m+nrl-NR_END));
}

void free_submatrix(float **b, long nrl, long nrh, long ncl, long nch)
/* free a submatrix allocated by submatrix() */
{
        free((FREE_ARG) (b+nrl-NR_END));
}

void free_convert_matrix(float **b, long nrl, long nrh, long ncl, long nch)
/* free a matrix allocated by convert_matrix() */
{
        free((FREE_ARG) (b+nrl-NR_END));
}

void free_f3tensor(float ***t, long nrl, long nrh, long ncl, long nch,
        long ndl, long ndh)
/* free a float f3tensor allocated by f3tensor() */
{
        free(t[nrl][ncl]+ndl-NR_END);
        free(t[nrl]+ncl-NR_END);
        free(t+nrl-NR_END);
}

#define NRANSI
#define ITMAX 100
#define CGOLD 0.3819660
#define ZEPS 1.0e-10
#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);

float brent(float ax, float bx, float cx, float (*f)(float), float tol,
        float *xmin)
{
        int iter;
        float a,b,d,etemp,fu,fv,fw,fx,p,q,r,tol1,tol2,u,v,w,x,xm;
        float e=0.0;

        a=(ax < cx ? ax : cx);
        b=(ax > cx ? ax : cx);
        x=w=v=bx;
        fw=fv=fx=(*f)(x);
        for (iter=1;iter<=ITMAX;iter++) {
                xm=0.5*(a+b);
                tol2=2.0*(tol1=tol*fabs(x)+ZEPS);
                if (fabs(x-xm) <= (tol2-0.5*(b-a))) {
                        *xmin=x;
                        return fx;
                }
                if (fabs(e) > tol1) {
                        r=(x-w)*(fx-fv);
                        q=(x-v)*(fx-fw);
                        p=(x-v)*q-(x-w)*r;
                        q=2.0*(q-r);
                        if (q > 0.0) p = -p;
                        q=fabs(q);
                        etemp=e;
                        e=d;
                        if (fabs(p) >= fabs(0.5*q*etemp) || p <= q*(a-x) || p >= q*(b-x))
                                d=CGOLD*(e=(x >= xm ? a-x : b-x));
                        else {
                                d=p/q;
                                u=x+d;
                                if (u-a < tol2 || b-u < tol2)
                                        d=SIGN(tol1,xm-x);
                        }
                } else {
                        d=CGOLD*(e=(x >= xm ? a-x : b-x));
                }
                u=(fabs(d) >= tol1 ? x+d : x+SIGN(tol1,d));
                fu=(*f)(u);
                if (fu <= fx) {
                        if (u >= x) a=x; else b=x;
                        SHFT(v,w,x,u)
                        SHFT(fv,fw,fx,fu)
                } else {
                        if (u < x) a=u; else b=u;
                        if (fu <= fw || w == x) {
                                v=w;
                                w=u;
                                fv=fw;
                                fw=fu;
                        } else if (fu <= fv || v == x || v == w) {
                                v=u;
                                fv=fu;
                        }
                }
        }
        nrerror("Too many iterations in brent");
        *xmin=x;
        return fx;
}

void fdjac(int n, float x[], float fvec[], float **df,
        void (*vecfunc)(int, float [], float []))
{
        int i,j;
        float h,temp,*f;

        f=vector(1,n);
        for (j=1;j<=n;j++) {
                temp=x[j];
                h=1.0e-4*fabs(temp);
                if (h == 0.0) h=1.0e-4;
                x[j]=temp+h;
                h=x[j]-temp;
                (*vecfunc)(n,x,f);
                x[j]=temp;
                for (i=1;i<=n;i++) df[i][j]=(f[i]-fvec[i])/h;
        }
        free_vector(f,1,n);
}

void qrupdt(float **r, float **qt, int n, float u[], float v[])
{
        void rotate(float **r, float **qt, int n, int i, float a, float b);
        int i,j,k;

        for (k=n;k>=1;k--) {
                if (u[k]) break;
        }
        if (k < 1) k=1;
        for (i=k-1;i>=1;i--) {
                rotate(r,qt,n,i,u[i],-u[i+1]);
                if (u[i] == 0.0) u[i]=fabs(u[i+1]);
                else if (fabs(u[i]) > fabs(u[i+1]))
                        u[i]=fabs(u[i])*sqrt(1.0+SQR(u[i+1]/u[i]));
                else u[i]=fabs(u[i+1])*sqrt(1.0+SQR(u[i]/u[i+1]));
        }
        for (j=1;j<=n;j++) r[1][j] += u[1]*v[j];
        for (i=1;i<k;i++)
                rotate(r,qt,n,i,r[i][i],-r[i+1][i]);
}

#define ALF 1.0e-4
#define TOLX 1.0e-7

void lnsrch(int n, float xold[], float fold, float g[], float p[], float x[],
        float *f, float stpmax, int *check, float(*func)(float[]))
{
        int i;
        float a,alam,alam2,alamin,b,disc,f2,fold2,rhs1,rhs2,slope,sum,temp,test,tmplam;

        *check=0;
        for (sum=0.0,i=1;i<=n;i++) sum += p[i]*p[i];
        sum=sqrt(sum);
        if (sum > stpmax)
                for (i=1;i<=n;i++) p[i] *= stpmax/sum;
        for (slope=0.0,i=1;i<=n;i++)
                slope += g[i]*p[i];
        test=0.0;
        for (i=1;i<=n;i++) {
                temp=fabs(p[i])/FMAX(fabs(xold[i]),1.0);
                if (temp > test) test=temp;
        }
        alamin=TOLX/test;
        alam=1.0;
        for (;;) {
                for (i=1;i<=n;i++) x[i]=xold[i]+alam*p[i];
                *f=(*func)(x);
                if (alam < alamin) {
                        for (i=1;i<=n;i++) x[i]=xold[i];
                        *check=1;
                        return;
                } else if (*f <= fold+ALF*alam*slope) return;
                else {
                        if (alam == 1.0)
                                tmplam = -slope/(2.0*(*f-fold-slope));
                        else {
                                rhs1 = *f-fold-alam*slope;
                                rhs2=f2-fold2-alam2*slope;
                                a=(rhs1/(alam*alam)-rhs2/(alam2*alam2))/(alam-alam2);
                                b=(-alam2*rhs1/(alam*alam)+alam*rhs2/(alam2*alam2))/(alam-alam2);
                                if (a == 0.0) tmplam = -slope/(2.0*b);
                                else {
                                        disc=b*b-3.0*a*slope;
                                        if (disc<0.0) nrerror("Roundoff problem in lnsrch.");
                                        else tmplam=(-b+sqrt(disc))/(3.0*a);
                                }
                                if (tmplam>0.5*alam)
                                        tmplam=0.5*alam;
                        }
                }
                alam2=alam;
                f2 = *f;
                fold2=fold;
                alam=FMAX(tmplam,0.1*alam);
        }
}

#define MAXITS 200
#define TOLF 1.0e-4
#define STPMX 100.0
#define TOLMIN 1.0e-6
#define FREERETURN {free_vector(fvec,1,n);free_vector(xold,1,n);\
        free_vector(w,1,n);free_vector(t,1,n);free_vector(s,1,n);\
        free_matrix(r,1,n,1,n);free_matrix(qt,1,n,1,n);free_vector(p,1,n);\
        free_vector(g,1,n);free_vector(fvcold,1,n);free_vector(d,1,n);\
        free_vector(c,1,n);return;}

int nn;
float *fvec;
void (*nrfuncv)(int n, float v[], float f[]);

void broydn(float x[], int n, int *check,
        void (*vecfunc)(int, float [], float []))
{
        void fdjac(int n, float x[], float fvec[], float **df,
                void (*vecfunc)(int, float [], float []));
        float fmin(float x[]);
        void lnsrch(int n, float xold[], float fold, float g[], float p[], float x[], float *f, float stpmax, int *check, float (*func)(float []));
        void qrdcmp(float **a, int n, float *c, float *d, int *sing);
        void qrupdt(float **r, float **qt, int n, float u[], float v[]);
        void rsolv(float **a, int n, float d[], float b[]);
        int i,its,j,k,restrt,sing,skip;
        float den,f,fold,stpmax,sum,temp,test,*c,*d,*fvcold;
        float *g,*p,**qt,**r,*s,*t,*w,*xold;

        c=vector(1,n);
        d=vector(1,n);
        fvcold=vector(1,n);
        g=vector(1,n);
        p=vector(1,n);
        qt=matrix(1,n,1,n);
        r=matrix(1,n,1,n);
        s=vector(1,n);
        t=vector(1,n);
        w=vector(1,n);
        xold=vector(1,n);
        fvec=vector(1,n);
        nn=n;
        nrfuncv=vecfunc;
        f=fmin(x);
        test=0.0;
        for (i=1;i<=n;i++)
                if (fabs(fvec[i]) > test)test=fabs(fvec[i]);
        if (test<0.01*TOLF) FREERETURN
        for (sum=0.0,i=1;i<=n;i++) sum += SQR(x[i]);
        stpmax=STPMX*FMAX(sqrt(sum),(float)n);
        restrt=1;
        for (its=1;its<=MAXITS;its++) {
                if (restrt) {
                  fdjac(n,x,fvec,r,vecfunc);
                        qrdcmp(r,n,c,d,&sing);
                        if (sing) nrerror("singular Jacobian in broydn");
                        for (i=1;i<=n;i++) {
                                for (j=1;j<=n;j++) qt[i][j]=0.0;
                                qt[i][i]=1.0;
                        }
                        for (k=1;k<n;k++) {
                                if (c[k]) {
                                        for (j=1;j<=n;j++) {
                                                sum=0.0;
                                                for (i=k;i<=n;i++)
                                                        sum += r[i][k]*qt[i][j];
                                                sum /= c[k];
                                                for (i=k;i<=n;i++)
                                                        qt[i][j] -= sum*r[i][k];
                                        }
                                }
                        }
                        for (i=1;i<=n;i++) {
                                r[i][i]=d[i];
                                for (j=1;j<i;j++) r[i][j]=0.0;
                        }
                } else {
                        for (i=1;i<=n;i++) s[i]=x[i]-xold[i];
                        for (i=1;i<=n;i++) {
                                for (sum=0.0,j=i;j<=n;j++) sum += r[i][j]*s[j];
                                t[i]=sum;
                        }
                        skip=1;
                        for (i=1;i<=n;i++) {
                                for (sum=0.0,j=1;j<=n;j++) sum += qt[j][i]*t[j];
                                w[i]=fvec[i]-fvcold[i]-sum;
                                if (fabs(w[i]) >= EPS*(fabs(fvec[i])+fabs(fvcold[i]))) skip=0;
                                else w[i]=0.0;
                        }
                        if (!skip) {
                                for (i=1;i<=n;i++) {
                                        for (sum=0.0,j=1;j<=n;j++) sum += qt[i][j]*w[j];
                                        t[i]=sum;
                                }
                                for (den=0.0,i=1;i<=n;i++) den += SQR(s[i]);
                                for (i=1;i<=n;i++) s[i] /= den;
                                qrupdt(r,qt,n,t,s);
                                for (i=1;i<=n;i++) {
                                        if (r[i][i] == 0.0) nrerror("r singular in broydn");
                                        d[i]=r[i][i];
                                }
                        }
                }
                for (i=1;i<=n;i++) {
                        for (sum=0.0,j=1;j<=n;j++) sum += qt[i][j]*fvec[j];
                        g[i]=sum;
                }
                for (i=n;i>=1;i--) {
                        for (sum=0.0,j=1;j<=i;j++) sum += r[j][i]*g[j];
                        g[i]=sum;
                }
                for (i=1;i<=n;i++) {
                        xold[i]=x[i];
                        fvcold[i]=fvec[i];
                }
                fold=f;
                for (i=1;i<=n;i++) {
                        for (sum=0.0,j=1;j<=n;j++) sum += qt[i][j]*fvec[j];
                        p[i] = -sum;
                }
                rsolv(r,n,d,p);
                lnsrch(n,xold,fold,g,p,x,&f,stpmax,check,fmin);
                test=0.0;
                for (i=1;i<=n;i++)
                        if (fabs(fvec[i]) > test) test=fabs(fvec[i]);
                if (test < TOLF) {
                        *check=0;
                        FREERETURN
                }
                if (*check) {
                        if (restrt) FREERETURN
                        else {
                                test=0.0;
                                den=FMAX(f,0.5*n);
                                for (i=1;i<=n;i++) {
                                        temp=fabs(g[i])*FMAX(fabs(x[i]),1.0)/den;
                                        if (temp > test) test=temp;
                                }
                                if (test < TOLMIN) FREERETURN
                                else restrt=1;
                        }
                } else {
                        restrt=0;
                        test=0.0;
                        for (i=1;i<=n;i++) {
                                temp=(fabs(x[i]-xold[i]))/FMAX(fabs(x[i]),1.0);
                                if (temp > test) test=temp;
                        }
                        if (test < TOLX) FREERETURN
                }
        }
        nrerror("MAXITS exceeded in broydn");
        FREERETURN
}

extern int nn;
extern float *fvec;
extern void (*nrfuncv)(int n, float v[], float f[]);

float fmin(float x[])
{
        int i;
        float sum;

        (*nrfuncv)(nn,x,fvec);
        for (sum=0.0,i=1;i<=nn;i++) sum += SQR(fvec[i]);
        return 0.5*sum;
}

void qrdcmp(float **a, int n, float *c, float *d, int *sing)
{
        int i,j,k;
        float scale=0.0,sigma,sum,tau;

        *sing=0;
        for (k=1;k<n;k++) {
                for (i=k;i<=n;i++) scale=FMAX(scale,fabs(a[i][k]));
                if (scale == 0.0) {
                        *sing=1;
                        c[k]=d[k]=0.0;
                } else {
                        for (i=k;i<=n;i++) a[i][k] /= scale;
                        for (sum=0.0,i=k;i<=n;i++) sum += SQR(a[i][k]);
                        sigma=SIGN(sqrt(sum),a[k][k]);
                        a[k][k] += sigma;
                        c[k]=sigma*a[k][k];
                        d[k] = -scale*sigma;
                        for (j=k+1;j<=n;j++) {
                                for (sum=0.0,i=k;i<=n;i++) sum += a[i][k]*a[i][j];
                                tau=sum/c[k];
                                for (i=k;i<=n;i++) a[i][j] -= tau*a[i][k];
                        }
                }
        }
        d[n]=a[n][n];
        if (d[n] == 0.0) *sing=1;
}

void rsolv(float **a, int n, float d[], float b[])
{
        int i,j;
        float sum;

        b[n] /= d[n];
        for (i=n-1;i>=1;i--) {
                for (sum=0.0,j=i+1;j<=n;j++) sum += a[i][j]*b[j];
                b[i]=(b[i]-sum)/d[i];
        }
}

void rotate(float **r, float **qt, int n, int i, float a, float b)
{
        int j;
        float c,fact,s,w,y;

        if (a == 0.0) {
                c=0.0;
                s=(b >= 0.0 ? 1.0 : -1.0);
        } else if (fabs(a) > fabs(b)) {
                fact=b/a;
                c=SIGN(1.0/sqrt(1.0+(fact*fact)),a);
                s=fact*c;
        } else {
                fact=a/b;
                s=SIGN(1.0/sqrt(1.0+(fact*fact)),b);
                c=fact*s;
        }
        for (j=i;j<=n;j++) {
                y=r[i][j];
                w=r[i+1][j];
                r[i][j]=c*y-s*w;
                r[i+1][j]=s*y+c*w;
        }
        for (j=1;j<=n;j++) {
                y=qt[i][j];
                w=qt[i+1][j];
                qt[i][j]=c*y-s*w;
                qt[i+1][j]=s*y+c*w;
        }
}


#define MAXIT 100
#define EULER 0.5772156649
#define FPMIN 1.0e-30

float expint(int n, float x)
{
        int i,ii,nm1;
        float a,b,c,d,del,fact,h,psi,ans;

        nm1=n-1;
        if (n < 0 || x < 0.0 || (x==0.0 && (n==0 || n==1)))
        standard_error("bad arguments in expint");
        else {
                if (n == 0) ans=exp(-x)/x;
                else {
                        if (x == 0.0) ans=1.0/nm1;

                        else {
                                if (x > 1.0) {
                                        b=x+n;
                                        c=1.0/FPMIN;
                                        d=1.0/b;
                                        h=d;
                                        for (i=1;i<=MAXIT;i++) {
                                                a = -i*(nm1+i);
                                                b += 2.0;
                                                d=1.0/(a*d+b);
                                                c=b+a/c;
                                                del=c*d;
                                                h *= del;
                                                if (fabs(del-1.0) < EPS) {
                                                        ans=h*exp(-x);
                                                        return ans;
                                                }
                                        }
                                        standard_error("continued fraction failed in expint");
                                } else {
                                        ans = (nm1!=0 ? 1.0/nm1 : -log(x)-EULER);
                                        fact=1.0;
                                        for (i=1;i<=MAXIT;i++) {
                                                fact *= -x/i;
                                                if (i != nm1) del = -fact/(i-nm1);
                                                else {
                                                        psi = -EULER;
                                                        for (ii=1;ii<=nm1;ii++) psi += 1.0/ii;
                                                        del=fact*(-log(x)+psi);
                                                }
                                                ans += del;
                                                if (fabs(del) < fabs(ans)*EPS) return ans;
                                        }
                                        standard_error("series failed in expint");
                                }
                        }
                }
        }
        return ans;
}

#define TINY 1.0e-20;

void ludcmp(float *a, int n, int *indx, float *d)

     // numerical recipes is poorly written.
{
        int i,imax,j,k;
        float big,dum,sum,temp;
        float *vv;

        vv=vector(0,n);
        *d=1.0;
        for (i=0;i<n;i++) {
                big=0.0;
                for (j=0;j<n;j++){
                  if ((temp=fabs(*(a+n*i+j))) > big) big=temp;
                }
                if (big == 0.0)
                    nrerror("Singular matrix in routine ludcmp");
                vv[i]=1.0/big;
        }
        for (j=0;j<n;j++) {
                for (i=0;i<j;i++) {
                        sum=*(a+n*i+j);
                        for (k=0;k<i;k++) sum -= *(a+n*i+k)*(*(a+n*k+j));
                        *(a+n*i+j)=sum;
                }
                big=0.0;
                for (i=j;i<n;i++) {
                        sum=*(a+n*i+j);
                        for (k=0;k<j;k++)
                                sum -= *(a+n*i+k)*(*(a+n*k+j));
                        *(a+n*i+j)=sum;
                        if ( (dum=vv[i]*fabs(sum)) >= big) {
                                big=dum;
                                imax=i;
                        }
                }
                if (j != imax) {
                        for (k=0;k<n;k++) {
                                dum=*(a+n*imax+k);
                                *(a+n*imax+k)=*(a+n*j+k);
                                *(a+n*j+k)=dum;
                        }
                        *d = -(*d);
                        vv[imax]=vv[j];
                }
                indx[j]=imax;
                if (*(a+n*j+j) == 0.0) *(a+n*j+j)=TINY;
                if (j != n) {
                        dum=1.0/(*(a+n*j+j));
                        for (i=j+1;i<n;i++) *(a+i*n+j) *= dum;
                }
        }
        free_vector(vv,0,n);
}

void lubksb(float *a, int n, int *indx, float b[])
{
        int i,ii=-1,ip,j;
        float sum;

        for (i=0;i<n;i++) {
                ip=indx[i];
                sum=b[ip];
                b[ip]=b[i];
                if (ii != -1)
                  for (j=ii;j<=i-1;j++) sum -= (*(a+n*i+j))*b[j];
                else if (sum) ii=i;
                b[i]=sum;
        }
        for (i=n-1;i>=0;i--) {
                sum=b[i];
                for (j=i+1;j<n;j++) sum -= (*(a+n*i+j))*b[j];
                b[i]=sum/(*(a+n*i+i));
        }
}
#undef JMAX
#undef MAXIT
#undef EPS
#undef FPMIN
#undef EULER

/* load energies from meshfiles in reverse order, from lo to hi
 * lethargy, converting to lethargy as we go. */
void load_meshes() {
        FILE *fh;
        char filename[256];
        int i;
        float val;

        sprintf(filename,MESH_PATH"%sth",SEP_CHAR);
        fh = fopen(filename,"r");
        if(fh == NULL){
                printf("failed to open file %s\n",filename);
                exit(1);
        }
        for(i=0;i<NTH+1;i++){
                fscanf(fh,"%f",&val);
                meshth[NTH-i]=log(MAX_E/val);
        }
        fclose(fh);

        sprintf(filename,MESH_PATH"%slo",SEP_CHAR);
        fh = fopen(filename,"r");
        if(fh == NULL){
                printf("failed to open file %s\n",filename);
                exit(1);
        }
        for(i=0;i<NLO+1;i++){
                fscanf(fh,"%f",&val);
                meshlh[NLOHI-i]=log(MAX_E/val);
        }
        fclose(fh);

        sprintf(filename,MESH_PATH"%shi",SEP_CHAR);
        fh = fopen(filename,"r");
        if(fh == NULL){
                printf("failed to open file %s\n",filename);
                exit(1);
        }
        for(i=0;i<NHI+1;i++){
                fscanf(fh,"%f",&val);
                meshlh[NHI-i]=log(MAX_E/val);
        }
        fclose(fh);

}

/* zero_actinide: takes a pointer to an actinide and initialises
 * it, setting all values to zero except the self-shielding factors,
 * which it sets to unity. */
void zero_actinide( actinide *act ){
        int i,j;

        for(i=0;i<NTH;i++){
                act->th.scat[i]=0.;
                act->th.absp[i]=0.;
                act->th.fiss[i]=0.;
                act->th.nubr[i]=0.;
                act->th.scos[i]=0.;
                for(j=0;j<NTH;j++){
                        act->th.sker[i][j]=0.;
                }
        }
        for(i=0;i<NLOHI;i++){
                act->lh.scat[i]=0.;
                act->lh.absp[i]=0.;
                act->lh.fiss[i]=0.;
                for(j=0;j<6;j++){
                        act->lh.ffct[i][j]=1.;
                }
                act->lh.inxs[i]=0.;
                act->lh.nubr[i]=0.;
                act->lh.scos[i]=0.;
        }
}

/* zero nonactinide: like zero_acinide, but for nonactinides */
void zero_nonactinide( nonactinide *nact ){
        int i,j;
        for(i=0;i<NTH;i++){
                nact->th.scat[i]=0.;
                nact->th.absp[i]=0.;
                nact->th.scos[i]=0.;
                for(j=0;j<NTH;j++){
                        nact->th.sker[i][j]=0.;
                }
        }
        for(i=0;i<NLOHI;i++){
                nact->lh.scat[i]=0.;
                nact->lh.absp[i]=0.;
                for(j=0;j<6;j++){
                        nact->lh.ffct[i][j]=1.;
                }
                nact->lh.scos[i]=0.;
        }
}

/* load_actinide_array_th: takes a pointer to one of the
 * arrays in an actinide, the actinide's name, and a
 * temperature.  it finds the appropriate file and parses
 * it, loading the data into the array. */
void load_actinide_array_th( float *branch,
                                         char *isotopedirname, char *localfilename ){
        int gr;
        float val;
        FILE *fh;
        char filename[256];

        sprintf(filename,"%sth%s%s",isotopedirname,SEP_CHAR,localfilename);

        /* load array from file */
        fh=fopen(filename,"r");
        if(fh == NULL){
/*              printf("failed to open file %s,\n\t"
                                                         "array will remain all zeros "
                                                         "as initialised.\n",filename);
*/      }else{
                while( fscanf(fh, "%d %e", &gr, &val) != EOF ){
                        if( gr > NTH || gr < 1 ){
                                printf("illegal group number %d in file %s\n",
                                                                         gr,filename);
                                exit(1);
                        }
                        /* reindex from hi to lo energy, lo to hi lethargy */
                        branch[NTH-gr]=val;
                }
                fclose(fh);
        }
        return;
}

/* load_actinide_array_lh: takes a pointer to one of the
 * arrays in an actinide, the actinide's name, and a
 * temperature.  it finds the appropriate files and parses
 * them, loading the data into the array. */
void load_actinide_array_lh( float *branch,
                                         char *isotopedirname, char *localfilename ){
        int gr;
        float val;
        FILE *fh;
        char filename[256];

        sprintf(filename,"%slo%s%s",isotopedirname,SEP_CHAR,localfilename);

        /* load array from file */
        fh=fopen(filename,"r");
        if(fh == NULL){
                //printf("failed to open file %s,\n\t"
                 //                                        "array will remain all zeros "
                 //                                        "as initialised.\n",filename);
        }else{
                while( fscanf(fh, "%d %e", &gr, &val) != EOF ){
                        if( gr > NLO || gr < 1 ){
                                printf("illegal group number %d in file %s\n",
                                                                         gr,filename);
                                exit(1);
                        }
                        /* reindex from hi to lo energy, lo to hi lethargy
                         * also reindex so ther is one index for lo and hi */
                        branch[NHI+NLO-gr]=val;
                }
                fclose(fh);
        }

        sprintf(filename,"%shi%s%s",isotopedirname,SEP_CHAR,localfilename);

        /* load array from file */
        fh=fopen(filename,"r");
        if(fh == NULL){
                printf("failed to open file %s,\n\t"
                                                         "array will remain all zeros "
                                                         "as initialised.\n",filename);
        }else{
                while( fscanf(fh, "%d %e", &gr, &val) != EOF ){
                        if( gr > NHI || gr < 1 ){
                                printf("illegal group number %d in file %s\n",
                                                                         gr,filename);
                                exit(1);
                        }
                        /* reindex from hi to lo energy, lo to hi lethargy */
                        branch[NHI-gr]=val;
                }
                fclose(fh);
        }

        return;
}

void verify_kernel(float kernel[][NTH], float sigma_s[NTH]) {

  int i,j;
  double sum=0;

  for(i=0; i<NTH; i++) {
    sum=0;
    for(j=0; j<NTH; j++) {
      sum+=kernel[i][j];
    }
    kernel[i][NTH-1]+=(sigma_s[i]-sum);
  }
}

/* load_actinide_matrix: takes a pointer to the scattering
 * kernel matrix in an actinide, the actinide's name, and a
 * temperature.  it finds the appropriate file and parses it,
 * loading the scattering kernel into the matrix. */
void load_actinide_matrix( float branch[][NTH],
                                         char *isotopedirname, char *localfilename ){
        int igr, jgr;
        float val;
        FILE *fh;
        char filename[256];

        sprintf(filename,"%sth%s%s",isotopedirname,SEP_CHAR,localfilename);

        /* load matrix from file */
        fh=fopen(filename,"r");
        if(fh == NULL){
                printf("failed to open file %s,\n\t"
                                                         "matrix will remain as initialised.\n",filename);
                return;
        }
        while( fscanf(fh, "%d %d %e", &igr, &jgr, &val) != EOF ){
                if( igr > NTH || igr < 1 || jgr > NTH || jgr < 1 ){
                        printf("illegal group number i=%d or "
                                                                 "j=%d in file %s\n", igr, jgr, filename);
                        exit(1);
                }
                branch[NTH-igr][NTH-jgr]=val;
        }
        fclose(fh);
        return;
}

/* load_actinide_factors: takes a pointer to the f-factor
 * arrays in an actinide, the actinide's name, and a temperature.
 * it finds the appropriate files and parses them, loading the self-
 * shielding factors into the array */
void load_actinide_factors( float branch[][6],
                                         char *isotopedirname, char *localfilename ){
        int gr;
        float val[6];
        FILE *fh;
        char filename[256];

        sprintf(filename,"%slo%s%s",isotopedirname,SEP_CHAR,localfilename);

        /* load array from file */
        fh=fopen(filename,"r");
        if(fh == NULL){
                printf("failed to open file %s\n",filename);
                printf("f-factor matrix will remain all ones "
                        "as initialized.\n");
        } else {
        while( fscanf(fh, "%d %e %e %e %e %e %e",
                                                                        &gr, val, val+1, val+2, val+3,
                                                                        val+4, val+5 ) != EOF ){
                if( gr > NLO || gr < 1 ){
                        printf("illegal group number %d in file %s\n",
                                                                 gr,filename);
                                exit(1);
                        }
                        memcpy(branch[NHI+NLO-gr],val,sizeof(branch[0]));
                }
                fclose(fh);

                sprintf(filename,"%shi%s%s",isotopedirname,SEP_CHAR,localfilename);

        /* load array from file */
                fh=fopen(filename,"r");
                if(fh == NULL){
                        printf("failed to open file %s\n",filename);
                        exit(1);
                }
                while( fscanf(fh, "%d %e %e %e %e %e %e",
                                                                  &gr, val, val+1, val+2, val+3,
                                                                  val+4, val+5) != EOF ){
                        if( gr > NHI || gr < 1 ){
                                printf("illegal group number %d in file %s\n",
                                                                 gr,filename);
                                exit(1);
                        }
                        memcpy(branch[NHI-gr],val,sizeof(branch[0]));
                }
                fclose(fh);
        }
        return;
}

/* load_inel: takes a pointer to the inkl matrix in an actinide, the
 * name of the actinide and loads the matrix from file. */
void load_inel(float mat[][NHI], char *isotopedirname,
                                                 char *localfilename){
        int igr, jgr;
        float val;
        FILE *fh;
        char filename[256];

        /* first initialise the matrix */
        for(igr=0;igr<NHI;igr++)for(jgr=0;jgr<NHI;jgr++)mat[igr][jgr]=0.;
        sprintf(filename,"%shi%s%s",isotopedirname,SEP_CHAR,localfilename);
        /* load kernel from file */
        fh=fopen(filename,"r");
        if(fh==NULL){
                printf("failed to open file %s\n\t\
                                                          inelastic kernel will remain as initialised.\n",\
                                                          filename);
                return;
        }
        while( fscanf(fh, "%d %d %e", &igr, &jgr, &val) != EOF){
                if(igr > NHI || jgr > NHI){
                        printf("illegal group number %d or %d in file %s\n",
                                                                 igr, jgr, filename);
                        exit(1);
                }
                if (jgr > igr) mat[NHI-igr][NHI-igr] += val;
                else mat[NHI-igr][NHI-jgr] += val;
        }
        fclose(fh);
        return;
}

/* load_actinide: takes a pointer to an actinide, it's name
 * and one of the temperatures we have data for, and loads
 * the information into the actinide from file */
void load_actinide( actinide *act, char *basename, float temp ){
        char secfilename[128];
        char isotopedirname[128];
/*      char miscfilepath[256];
        FILE *fh;
        float t_half;
        int mass_num;
*/
        /* initialise the actinide before storing values, since
         * if the value is zero, endf will not have reported it */
        zero_actinide(act);
        sprintf(isotopedirname,ACTINIDE_PATH"%s%s%s",SEP_CHAR,basename,SEP_CHAR);
        /* first load the mass number and decay constant */
/*      sprintf(miscfilepath,ACTINIDE_PATH"/%s/misc",basename);
        fh=fopen(miscfilepath,"r");
        if(fh == NULL){
                printf("failed to open misc file %s containing mass number "
                                                         "and half life for %s.",miscfilepath,basename);
                exit(1);
        }
        fscanf(fh,"%d %e", &mass_num, &t_half);
        act->a = mass_num;
        act->lambda = M_LN2 / t_half / 365.;
        fclose(fh);
*/
        /* we pass pointers to the relevant piece of act only
         * always using the appropriate extraction function */

        /* load scattering cross section */

        sprintf(secfilename,"output.%d.3.2",(int)temp);
        load_actinide_array_th(act->th.scat,isotopedirname,secfilename);
        load_actinide_array_lh(act->lh.scat,isotopedirname,secfilename);

        /* load inelastic scattering cross section */
        sprintf(secfilename,"output.%d.3.4",(int)temp);
        load_actinide_array_lh(act->lh.inxs,isotopedirname,secfilename);

        /* load absorption cross section */
        sprintf(secfilename,"output.%d.3.102",(int)temp);
        load_actinide_array_th(act->th.absp,isotopedirname,secfilename);
        load_actinide_array_lh(act->lh.absp,isotopedirname,secfilename);

        /* load fission cross section */
        sprintf(secfilename,"output.%d.3.18",(int)temp);
        load_actinide_array_th(act->th.fiss,isotopedirname,secfilename);
        load_actinide_array_lh(act->lh.fiss,isotopedirname,secfilename);

        /* load nubar */
        sprintf(secfilename,"output.%d.3.452",(int)temp);
        load_actinide_array_th(act->th.nubr,isotopedirname,secfilename);
        load_actinide_array_lh(act->lh.nubr,isotopedirname,secfilename);

        /* load scattering cosines */
        sprintf(secfilename,"output.%d.3.251",(int)temp);
        load_actinide_array_th(act->th.scos,isotopedirname,secfilename);
        load_actinide_array_lh(act->lh.scos,isotopedirname,secfilename);

        /* load scattering kernel */
        sprintf(secfilename,"output.%d.6.221",(int)temp);
        load_actinide_matrix(act->th.sker,isotopedirname,secfilename);
        verify_kernel(act->th.sker,act->th.scat);

        /* load self-shielding factors */
        sprintf(secfilename,"output.%d.3.1",(int)temp);
        load_actinide_factors(act->lh.ffct,isotopedirname,secfilename);

#ifdef USE28INELSCATKERNEL
        /* copy global inelastic scattering kernel from uranium
         * 238 to inkl*/
        memcpy(&act->lh.inkl,&globalinel,sizeof(globalinel));

#endif
        return;
}

/* load_nonactinide: like load_actinide, but for nonactinides */
void load_nonactinide( nonactinide *nact, char *basename, float temp ){
        char secfilename[64];
        char isotopedirname[64];
/*      char miscfilepath[256];
        int mass_num;
        FILE *fh;
*/
        /* initialise the actinide before storing values, since
         * if the value is zero, endf will not have reported it */
        zero_nonactinide(nact);
        sprintf(isotopedirname,ACTINIDE_PATH"%s%s%s",SEP_CHAR,basename,SEP_CHAR);

        /* first load the mass number */
/*      sprintf(miscfilepath,ACTINIDE_PATH"/%s/misc",basename);
        fh=fopen(miscfilepath,"r");
        if(fh == NULL){
                printf("failed to open misc file %s containing mass number "
                                                         "for %s.",miscfilepath,basename);
                exit(1);
        }
        fscanf(fh,"%d", &mass_num);
        nact->a = mass_num;
        fclose(fh);
*/
        /* we pass pointers to the relevant piece of act only
         * always using the appropriate extraction function */
        /* load scattering cross section */
        sprintf(secfilename,"output.%d.3.2",(int)temp);
        load_actinide_array_th(nact->th.scat,isotopedirname,secfilename);
        load_actinide_array_lh(nact->lh.scat,isotopedirname,secfilename);

        /* load absorption cross section */
        sprintf(secfilename,"output.%d.3.102",(int)temp);
        load_actinide_array_th(nact->th.absp,isotopedirname,secfilename);
        load_actinide_array_lh(nact->lh.absp,isotopedirname,secfilename);

        /* load scattering cosines */
        sprintf(secfilename,"output.%d.3.251",(int)temp);
        load_actinide_array_th(nact->th.scos,isotopedirname,secfilename);
        load_actinide_array_lh(nact->lh.scos,isotopedirname,secfilename);

        /* load scattering kernel */
        sprintf(secfilename,"output.%d.6.221",(int)temp);
        load_actinide_matrix(nact->th.sker,isotopedirname,secfilename);
        verify_kernel(nact->th.sker,nact->th.scat);

        /* load self-shielding factors */
        sprintf(secfilename,"output.%d.3.1",(int)temp);
        load_actinide_factors(nact->lh.ffct,isotopedirname,secfilename);

        return;
}

/* load_bound_nonactinide: like load_actinide, but for bound nonactinides
 * note: this fuction only loads thermal data, leaviing the rest of the
 * structure zeroed */
void load_bound_nonactinide( nonactinide *nact, char *basename,
                                         char *bound_name, float temp ){
        char secfilename[64];
        char isotopedirname[64];
/*      char miscfilepath[256];
        int mass_num;
        FILE *fh;
*/
        /* initialise the actinide before storing values, since
         * if the value is zero, endf will not have reported it */
        zero_nonactinide(nact);
        sprintf(isotopedirname,ACTINIDE_PATH"%s%s.%s%s",SEP_CHAR,basename,bound_name,SEP_CHAR);

        /* first load the mass number */
/*      sprintf(miscfilepath,ACTINIDE_PATH"/%s/misc",basename);
        fh=fopen(miscfilepath,"r");
        if(fh == NULL){
                printf("failed to open misc file %s containing mass number "
                                                         "for %s.",miscfilepath,basename);
                exit(1);
        }
        fscanf(fh,"%d", &mass_num);
        nact->a = mass_num;
        fclose(fh);
*/
        /* we pass pointers to the relevant piece of act only
         * always using the appropriate extraction function */

        /* load scattering cross section */
        sprintf(secfilename,"output.%d.3.22x",(int)temp);
        load_actinide_array_th(nact->th.scat,isotopedirname,secfilename);

        /* load absorption cross section */
        sprintf(secfilename,"output.%d.3.10x",(int)temp);
        load_actinide_array_th(nact->th.absp,isotopedirname,secfilename);

        /* load scattering cosines */
        sprintf(secfilename,"output.%d.3.251",(int)temp);
        load_actinide_array_th(nact->th.scos,isotopedirname,secfilename);

        /* load scattering kernel */
        sprintf(secfilename,"output.%d.6.22x",(int)temp);
        load_actinide_matrix(nact->th.sker,isotopedirname,secfilename);
        verify_kernel(nact->th.sker,nact->th.scat);

        return;
}

/* interpolate_actinide: takes a pointer to an actinide object,
 * its name, and a temperature and interpoltes between the two
 * nearest sampled temperatures for every value in the actinide
 * and stores the interpolated values in the actinide */
void interpolate_actinide(actinide *act, char *basename, float temp ){

        actinide *hotter, *cooler;
        float htemp, ltemp, hval, lval;
        int i,j;
        int tmpind=0;
        char miscfilepath[256];
        FILE *fh;
        float *loc_temps=temps;
        int loc_ntemps=NTEMPS;
        int mass_num;
        float t_half;
        float factor[3][2];
        // 0 = scat, 1 = abs, 2 = fiss, // 0 = fast, 1 = therm
        /* don't bother doing interpolation for lambda and mass num
         * first load the mass number and decay constant */
        hotter = (actinide *) malloc( sizeof(actinide) );
        cooler = (actinide *) malloc( sizeof(actinide) );
        sprintf(miscfilepath,ACTINIDE_PATH"%s%s%smisc",SEP_CHAR,basename,SEP_CHAR);
        fh=fopen(miscfilepath,"r");
        if(fh == NULL){
                printf("failed to open misc file %s containing mass number "
                                                         "and half life for %s.",miscfilepath,basename);
                exit(1);
        }
        fscanf(fh,"%d %e", &mass_num, &t_half);
        act->a = mass_num;
        act->lambda = M_LN2 / t_half / 365.; /* lambda in 1/days */
        fclose(fh);
#ifdef USE28INELSCATKERNEL
        /* use the u238 inelastic scattering kernel */
        memcpy(&act->lh.inkl,&globalinel,sizeof(globalinel));
#endif

        /* don't attempt to work with temperatures outside the range
         * for which we have data */
        if( temp < loc_temps[0] || temp > loc_temps[NTEMPS-1] ){
                printf("requested temperature %.0f outside of allowed range "
                                                         "%.0f-%.0f\n", temp, loc_temps[0],
                                                         loc_temps[loc_ntemps-1]);
                exit(1);
        }

        /* choose the temperatures just above and below the requested
         * temperature and call them htemp and ltemp */
        htemp = 0;
        while( htemp < temp ) htemp = loc_temps[tmpind++];
        //if( temp == htemp ){
        //      load_actinide( act, basename, temp );
        //      return;
        //}
        ltemp=loc_temps[tmpind-2];
        load_actinide( hotter, basename, htemp );
        load_actinide( cooler, basename, ltemp );
        if(basename==act_basenames[I_U_238]) {
          factor[0][0]=1.0;
          factor[0][1]=1.0;
          factor[1][0]=1.0;
          factor[1][1]=1.0;
          factor[2][0]=0.70;
          factor[2][1]=1.0;
        }
        else if(basename==act_basenames[I_PU_240]) {
          factor[0][0]=1.0;
          factor[0][1]=1.0;
          factor[1][0]=1.30;
          factor[1][1]=1.075;
          factor[2][0]=1.0;
          factor[2][1]=1.0;
        }
        else if(basename==act_basenames[I_AM_241]) {
          factor[0][0]=1.0;
          factor[0][1]=1.0;
          factor[1][0]=1.11;
          factor[1][1]=1.11;
          factor[2][0]=1.0;
          factor[2][1]=1.0;
        }
        else if(basename==act_basenames[I_PU_241]) {
          factor[0][0]=1.0;
          factor[0][1]=1.0;
          factor[1][0]=1.0;
          factor[1][1]=1.0;
          factor[2][0]=1.0;
          factor[2][1]=1.0;
        }
        else {
          for(i=0; i<3; i++) {
            factor[i][0]=1.;
            factor[i][1]=1.;
          }
        }
        if(basename==act_basenames[I_TH_230] ||
           basename==act_basenames[I_TH_232] ||
           basename==act_basenames[I_PA_231] ||
           basename==act_basenames[I_U_234] ||
           basename==act_basenames[I_U_236] ||
           basename==act_basenames[I_U_238] ||
           basename==act_basenames[I_NP_237] ||
           basename==act_basenames[I_PU_236] ||
           basename==act_basenames[I_PU_238] ||
           basename==act_basenames[I_PU_240] ||
           basename==act_basenames[I_PU_242] ||
           basename==act_basenames[I_AM_241] ||
           basename==act_basenames[I_AM_243] ||
           basename==act_basenames[I_CM_242] ||
           basename==act_basenames[I_CM_244] ||
           basename==act_basenames[I_CM_246]) {
                factor[2][0]=factor[2][0]*0.875;
                factor[2][1]=factor[2][1]*0.875;
           }
        /* for every value in act, interpolate between the the values
         * of hotter and cooler */
        for(i=0;i<NTH;i++){
                hval=hotter->th.scat[i]*factor[0][1];
                lval=cooler->th.scat[i]*factor[0][1];
                act->th.scat[i] = lval +
                                ( temp - ltemp ) * ( hval - lval ) / ( htemp - ltemp );
                hval=hotter->th.absp[i]*factor[1][1];
                lval=cooler->th.absp[i]*factor[1][1];
                act->th.absp[i] = lval +
                                ( temp - ltemp ) * ( hval - lval ) / ( htemp - ltemp );
                hval=hotter->th.fiss[i]*factor[2][1];
                lval=cooler->th.fiss[i]*factor[2][1];
                act->th.fiss[i] = lval +
                                ( temp - ltemp ) * ( hval - lval ) / ( htemp - ltemp );
                hval=hotter->th.nubr[i];
                lval=cooler->th.nubr[i];
                act->th.nubr[i] = lval +
                                ( temp - ltemp ) * ( hval - lval ) / ( htemp - ltemp );
                hval=hotter->th.scos[i];
                lval=cooler->th.scos[i];
                act->th.scos[i] = lval +
                                ( temp - ltemp ) * ( hval - lval ) / ( htemp - ltemp );
                for(j=0;j<NTH;j++){
                        hval=hotter->th.sker[i][j]*factor[0][1];
                        lval=cooler->th.sker[i][j]*factor[0][1];
                        act->th.sker[i][j] = lval +
                                        ( temp - ltemp ) * ( hval-lval ) / ( htemp - ltemp );
                }
        }

        for(i=0;i<NLOHI;i++){
                hval=hotter->lh.scat[i]*factor[0][0];
                lval=cooler->lh.scat[i]*factor[0][0];
                act->lh.scat[i] = lval +
                                ( temp - ltemp ) * ( hval - lval ) / ( htemp - ltemp );
                hval=hotter->lh.inxs[i]*factor[1][0];
                lval=cooler->lh.inxs[i]*factor[1][0];
                act->lh.inxs[i] = lval +
                                ( temp - ltemp ) * ( hval - lval ) / ( htemp - ltemp );
                hval=hotter->lh.absp[i]*factor[1][0];
                lval=cooler->lh.absp[i]*factor[1][0];
                act->lh.absp[i] = lval +
                                ( temp - ltemp ) * ( hval - lval ) / ( htemp - ltemp );
                hval=hotter->lh.fiss[i]*factor[2][0];
                lval=cooler->lh.fiss[i]*factor[2][0];
                act->lh.fiss[i] = lval +
                                ( temp - ltemp ) * ( hval - lval ) / ( htemp - ltemp );
                hval=hotter->lh.nubr[i];
                lval=cooler->lh.nubr[i];
                act->lh.nubr[i] = lval +
                                ( temp - ltemp ) * ( hval - lval ) / ( htemp - ltemp );
                hval=hotter->lh.scos[i];
                lval=cooler->lh.scos[i];
                act->lh.scos[i] = lval +
                                ( temp - ltemp ) * ( hval - lval ) / ( htemp - ltemp );
                for(j=0;j<6;j++){
                        hval=hotter->lh.ffct[i][j];
                        lval=cooler->lh.ffct[i][j];
                        act->lh.ffct[i][j] = lval +
                                        ( temp - ltemp ) * ( hval-lval ) / ( htemp - ltemp );
                }
        }
        // Jan 19 Andy Osborne, eliminate mem leak
        free(hotter);
        free(cooler);
        return;
}

/* interpolate_nonactinide: like interpolate_actinide, but for nonactinides */
void interpolate_nonactinide( nonactinide *nact, char *basename,
                                         float temp, char *bound_name ){
        nonactinide hotter, cooler;
        float htemp, ltemp, hval, lval, temp_temp;
        float *loc_temps=temps;
        int loc_ntemps=NTEMPS;
        int i,j;
        int tmpind=0;
        FILE *fh;
        char miscfilepath[256];
        int mass_num;

        temp_temp=-1;
        /* don't bother to interpolate the mass number */
        /* first load the mass number */
        sprintf(miscfilepath,ACTINIDE_PATH"%s%s%smisc",SEP_CHAR,basename,SEP_CHAR);
        fh=fopen(miscfilepath,"r");
        if(fh == NULL){
                printf("failed to open misc file %s containing mass number "
                                                         "for %s.",miscfilepath,basename);
                exit(1);
        }
        fscanf(fh,"%d", &mass_num);
        nact->a = mass_num;
        fclose(fh);

        /* first zero the nonactinide */
        zero_nonactinide(nact);

        /* don't attempt to work with temperatures outside the range
         * for which we have data */
        if( temp < loc_temps[0] || temp > loc_temps[loc_ntemps-1] ){
                printf("requested temperature %.0f outside of allowed range "
                                                         "%.0f-%.0f\n", temp, loc_temps[0],
                                                         loc_temps[loc_ntemps-1]);
                exit(1);
        }

        /* choose the temperatures just above and below the requested
         * temperature and call them htemp and ltemp */
        htemp = 0;
        while( htemp < temp ) htemp = loc_temps[tmpind++];
        ltemp=loc_temps[tmpind-2];

        load_nonactinide( &hotter, basename, htemp );
        load_nonactinide( &cooler, basename, ltemp );

        /* for every value in act, interpolate between the the values
         * of hotter and cooler */
        for(i=0;i<NLOHI;i++){
                hval=hotter.lh.scat[i];
                lval=cooler.lh.scat[i];
                nact->lh.scat[i] = lval +
                                ( temp - ltemp ) * ( hval - lval ) / ( htemp - ltemp );
                hval=hotter.lh.absp[i];
                lval=cooler.lh.absp[i];
                nact->lh.absp[i] = lval +
                                ( temp - ltemp ) * ( hval - lval ) / ( htemp - ltemp );
                hval=hotter.lh.scos[i];
                lval=cooler.lh.scos[i];
                nact->lh.scos[i] = lval +
                                ( temp - ltemp ) * ( hval - lval ) / ( htemp - ltemp );
                for(j=0;j<6;j++){
                        hval=hotter.lh.ffct[i][j];
                        lval=cooler.lh.ffct[i][j];
                        nact->lh.ffct[i][j] = lval +
                                        ( temp - ltemp ) * ( hval-lval ) / ( htemp - ltemp );
                }
        }

        /* if nuclide is bound, use the temps for bound nuclides,
         * but only for the thermal stuff */
        if( bound_name != NULL ) {
                loc_ntemps=NBOUNDTEMPS;
                loc_temps=bound_temps;

                /* don't attempt to work with temperatures outside the range
                 * for which we have data */
                if( temp < loc_temps[0] || temp > loc_temps[loc_ntemps-1] ){
                        printf("WARNING: requested temperature %.0f outside of allowed range "
                                                                 "for bound nuclides "
                                                                 "%.0f-%.0f\n", temp, loc_temps[0],
                                                                 loc_temps[loc_ntemps-1]);
                        if(temp < loc_temps[0]) {
                        	exit(1);
                        }
                        else {  /* ADDED 11/11/05: Thermal data for bound nuclides extends to 1000 K only.
                        	 * If user wishes to run at a higher temperature, print a warning and
                        	 * use the 1000K values. */
                        	 printf("Defaulting to %.0f K values for thermal cross sections.\n",
                        	                                 loc_temps[loc_ntemps-1]);
                        	 temp_temp=temp;
                        	 temp=999.99;
                        }
                }
                /* choose the temperatures just above and below the requested
                 * temperature and call them htemp and ltemp */
                htemp = 0;
                tmpind = 0;
                while( htemp < temp ) htemp = loc_temps[tmpind++];
                ltemp=loc_temps[tmpind-2];

                //printf("%.0f %.0f %.0f\n",ltemp, temp, htemp);
                load_bound_nonactinide( &hotter, basename, bound_name, htemp );
                load_bound_nonactinide( &cooler, basename, bound_name, ltemp );
        }

        for(i=0;i<NTH;i++){
                hval=hotter.th.scat[i];
                lval=cooler.th.scat[i];
                nact->th.scat[i] = lval +
                                ( temp - ltemp ) * ( hval - lval ) / ( htemp - ltemp );
                hval=hotter.th.absp[i];
                lval=cooler.th.absp[i];
                nact->th.absp[i] = lval +
                                ( temp - ltemp ) * ( hval - lval ) / ( htemp - ltemp );
                hval=hotter.th.scos[i];
                lval=cooler.th.scos[i];
                nact->th.scos[i] = lval +
                                ( temp - ltemp ) * ( hval - lval ) / ( htemp - ltemp );
                for(j=0;j<NTH;j++){
                        hval=hotter.th.sker[i][j];
                        lval=cooler.th.sker[i][j];
                        nact->th.sker[i][j] = lval +
                                        ( temp - ltemp ) * ( hval-lval ) / ( htemp - ltemp );
                }
        }
        if(temp_temp > 0.) {    // ADDED 11/11/05: Reset temperature to requested value
        	temp=temp_temp;
        }
        return;
}

void set_all_actinides( float temp ) {
        int i;
        actinide *ptr;
        for(i=0;i<NACT;i++) {

            // Jan 19 Andy Osborne, eliminate memory leak
                ptr = get_actinide(act_basenames[i], temp);
                actinide_array[i] = *ptr;
                free(ptr);
                //actinide_array[i] = *get_actinide(act_basenames[i], temp);

        }
        return;
}

/* get_actinide: given a name for the actinide and the temperature,
 * allocate memory for the actinide and populate its values.  Returns
 * a pointer to the actinide */
actinide *get_actinide(char *basename, float temp) {

        actinide *actptr;
        actptr = (actinide *) malloc( sizeof(actinide) );
        if( actptr == 0 ){
                printf("not enough memory to load %s.\n", basename);
                exit(1);
        }
        interpolate_actinide( actptr, basename, temp);
        return actptr;
}

/* get_nonactinide: given a name for the nonactinide, a temperature, and
 * the name of the compound it's bound in, allocate memory for a nonactinide
 * and give it the correct values.  return a pointer to the nonactinide.
 * if the nuclide is not bound, pass NULL for bound_name */
// Feb 03 Andy O - memory is now preallocated and the pointer of interest is passed to the function.
// this lets us free the memory to eliminate a memory leak (about 30kb per allocation)
nonactinide* get_nonactinide(char *basename, float temp, char *bound_name, nonactinide *nonActPtr) {
        /*
        nonactinide *nactptr;
        nactptr = (nonactinide *) malloc( sizeof(nonactinide) );
        if( nactptr == 0 ){
                printf("not enough memory to load %s.\n", basename);
                exit(1);
        } */
        interpolate_nonactinide( nonActPtr, basename, temp, bound_name );
        return nonActPtr; // return address of the pointer fed in.  Just for convenience.
        //return nactptr;

}

void load_fiss_spec(){
        FILE *fh;
        int i,gr;
        float val;
        char a_path[100];

        for(i=0;i<NLOHI;i++) fiss_spec[i]=0.;
        sprintf(a_path,ACTINIDE_PATH "%sfiss_spec_hi",SEP_CHAR);
        fh=fopen(a_path,"r");
        if( fh == NULL ){
                printf("failed to open" ACTINIDE_PATH "\fiss_spec_hi,\n\t"
                                                         "fiss_spec will remain zero\n" );
        }else{
                for(i=0;i<NLOHI;i++){
                        fscanf(fh,"%d %e",&gr,&val);
                        if( gr < 1 || gr > NHI ){
                                //fprintf( stderr, "illegal group number %d in "
                                //                                    ACTINIDE_PATH "\fiss_spec_hi,\n", gr);
                                exit(1);
                        }
                        fiss_spec[NHI-gr]=val;
                }
                fclose(fh);
        }
        return;
}

void decay_step( float *N_vals, float interval ) {

  // step forward in time at zero power (decay only)

        int i;
//        int rxn_paths[NACT] = { 1,2,3,4,15,8,15,8,9,10,11,18,14,1,0,7,17,
//                18,21,20,21,22,23,-1 };
        int decay_paths[NACT] = { -1,-1,12,14,13,2,0,2,3,4,16,5,-1,-1,-1,
                1,15,-1,8,7,8,9,10,11 };
        float dN[NACT]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

        for(i=0;i<NACT;i++){
                /* for every decaying nuclide, decrement the number of decaying
                 * nuclei and increment the number of decay product nuclei, if
                 * any, proportional to the decay rate, the time interval
                 * and the number of nuclei initially present */
                dN[i] -= N_vals[i] * actinide_array[i].lambda * interval;
                if(decay_paths[i] > -1) dN[decay_paths[i]] += N_vals[i] *
                                          actinide_array[i].lambda * interval;
        }

        /* treat am-242m specially, since it has two decay branches */
        dN[I_PU_242] += 0.173 * N_vals[I_AM_242] *
                          actinide_array[I_AM_242].lambda * interval;
        dN[I_CM_242] += 0.827 * N_vals[I_AM_242] *
                          actinide_array[I_AM_242].lambda * interval;

        for(i=0;i<NACT;i++) N_vals[i] += dN[i];
        return;
}

void step_forward( float *N_vals, float *sig_phi, float *sigf_phi, float interval ) {

  // sig_phi and sigf_phi are microscopic cross sections * fluxes

        int i;
        int rxn_paths[NACT] = { 1,2,3,4,15,8,15,8,9,10,11,18,14,1,0,7,17,
                18,21,20,21,22,23,-1 };
        int decay_paths[NACT] = { -1,-1,12,14,13,2,0,2,3,4,16,5,-1,-1,-1,
                1,15,-1,8,7,8,9,10,11 };
        float dN[NACT]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

        for(i=0;i<NACT;i++){

                /* for every reacting nuclide, decrement the number of reactant
                 * nuclei and increment the number of product nuclei proportional
                 * to the reaction rate sig_phi */
                dN[i] -= (sig_phi[i]+sigf_phi[i])*N_vals[i] * interval;
                if(i==I_U_238) {  // temporary u-238 (n,2n) fudge
                  dN[rxn_paths[i]] += sig_phi[i]*N_vals[i] * interval*.9935;//.9885;  // n,gamma
                    dN[I_NP_237] += sig_phi[i]*N_vals[i] * interval*.0065;//.0115;   // n,2n
                } else if(i==I_AM_241) {   // temporary am241 -> am242 ground state fudge
                  dN[rxn_paths[i]] += sig_phi[i]*N_vals[i] * interval * 0.15;
                  dN[I_PU_242] += sig_phi[i]*N_vals[i] * interval * 0.85 * 0.173;
                  dN[I_CM_242] += sig_phi[i]*N_vals[i] * interval * 0.85 * 0.827;
                }
                else if (rxn_paths[i] > -1) dN[rxn_paths[i]] += sig_phi[i]*N_vals[i] * interval;

                /* for every decaying nuclide, decrement the number of decaying
                 * nuclei and increment the number of decay product nuclei, if
                 * any, proportional to the decay rate, the time interval
                 * and the number of nuclei initially present */
                dN[i] -= N_vals[i] * actinide_array[i].lambda * interval;
                if(decay_paths[i] > -1) dN[decay_paths[i]] += N_vals[i] *
                                          actinide_array[i].lambda * interval;
        }

        /* treat am-242m specially, since it has two decay branches */
        dN[I_PU_242] += 0.173 * N_vals[I_AM_242] *
                          actinide_array[I_AM_242].lambda * interval;
        dN[I_CM_242] += 0.827 * N_vals[I_AM_242] *
                          actinide_array[I_AM_242].lambda * interval;

        for(i=0;i<NACT;i++) N_vals[i] += dN[i];
        return;
}

int get_case() {

  // gets the case number to be run, cases as defined in Run struct above
  int c=0,innum=1,c1,c2,c3,c4;
  int valid=0;

  printf("Please select general fuel cycle option from\n");
  printf("0) Standard U-Pu Scenario\n");
  while(valid != 1) {
    fscanf(stdin,"%d",&innum);
    switch (innum) {
    case 0:
      c=0;
      valid=1;
      break;
    default:
      printf("Invalid input.\n");
      break;
    }
  }
  valid=0;
  printf("Please select the number of times (less than or\n");
  printf("equal to 2) that plutonium is to be recycled:\n");
  printf("Select (0) for: reprocess, but do not reuse\n");
  printf("Select (-1) for: equilibrium MOX cycle\n");
  while(valid != 1) {
    fscanf(stdin,"%d",&innum);
    switch (innum) {
    case 0:
      valid=1;
      break;
    case 1:
      valid=1;
      c=c+100;
      break;
    case 2:
      valid=1;
      c=c+200;
      break;
    case -1:
      valid=1;
      printf("You have selected case 0004\n");
      return(4);
      break;
    default:
      printf("Invalid input.\n");
      break;
    }
  }
  valid=0;
  printf("Please select the number of times (less than or\n");
  printf("equal to 2) that uranium is to be recycled:\n");
  printf("Select (0) for: reprocess, but do not reuse\n");
  while(valid != 1) {
    fscanf(stdin,"%d",&innum);
    switch (innum) {
    case 0:
      valid=1;
      break;
    case 1:
      valid=1;
      c=c+10;
      break;
    case 2:
      valid=1;
      c=c+20;
      break;
    default:
      printf("Invalid input.\n");
      break;
    }
  }
  valid=0;
  printf("Please select the uranium matrix for plutonium fuel:\n");
  printf("0) Depleted uranium\n");
  printf("1) Natural uranium\n");
  printf("2) Recycled uranium\n");
  printf("3) Recycled uranium (blend variant)\n");
  while(valid != 1) {
    fscanf(stdin,"%d",&innum);
    switch (innum) {
    case 0:
      valid=1;
      break;
    case 1:
      valid=1;
      c=c+1;
      break;
    case 2:
      valid=1;
      c=c+2;
      break;
    case 3:
      valid=1;
      c=c+3;
      break;
    default:
      printf("Invalid input.\n");
      break;
    }
  }
  c1=c/1000;
  c2=c/100-c1*10;
  c3=c/10-c1*100-c2*10;
  c4=c-c1*1000-c2*100-c3*10;
  printf("You have selected case %i%i%i%i\n",c1,c2,c3,c4);
  return(c);
}

void echo_reactor_choice() {

  // outputs particulars regarding the user's reactor choice to the screen

  Run.Reactor=Run.ReactorFront;
  printf("Reactor: %s Geomtype %d FissionReg %d\n",Run.Reactor->Name,Run.Reactor->geomtype,Run.Reactor->Fission_source_region);
  printf("Power [MWt]: %f Inventory [kgHM]: %f effic: %f load factor: %f power density [MWt/kg]: %f\n",Run.Reactor->power,Run.Reactor->inventory,Run.Reactor->effic,Run.Reactor->load_factor,Run.Reactor->pow_den);
  printf("Core: radius [cm]: %f height [cm]: %f volratio V0/V1: %f equiv radius [cm]: %f\n",Run.Reactor->core_r,Run.Reactor->core_h,Run.Reactor->volratio,Run.Reactor->core_r_equiv);
  printf("Region 0 (+clad) radius [cm]: %f temp [K]: %f\n",Run.Reactor->r[0],Run.Reactor->T[0]);
  printf("Region 0 contents: [name density[g/cm^3] volfraction]\n");
  Run.Reactor->Matl[0]=Run.Reactor->MatlFront[0];
  while (Run.Reactor->Matl[0] != NULL) {
    printf("%s %f %f\n",Run.Reactor->Matl[0]->Name,Run.Reactor->Matl[0]->density,Run.Reactor->Matl[0]->volpct);
    Run.Reactor->Matl[0]=Run.Reactor->Matl[0]->NextMatl;
  }
  printf("Region 1 radius [cm]: %f temp [K]: %f\n",Run.Reactor->r[1],Run.Reactor->T[1]);
  printf("Region 1 contents: [name density[g/cm^3] volfraction]\n");
  Run.Reactor->Matl[1]=Run.Reactor->MatlFront[1];
  while (Run.Reactor->Matl[1] != NULL) {
    printf("%s %f %f\n",Run.Reactor->Matl[1]->Name,Run.Reactor->Matl[1]->density,Run.Reactor->Matl[1]->volpct);
    Run.Reactor->Matl[1]=Run.Reactor->Matl[1]->NextMatl;
  }
  printf("Control absorber materials:\n");
  Run.Reactor->Matl[2]=Run.Reactor->MatlFront[2];
  while (Run.Reactor->Matl[2] != NULL) {
    printf("%s\n",Run.Reactor->Matl[2]->Name);
    Run.Reactor->Matl[2]=Run.Reactor->Matl[2]->NextMatl;
  }
  printf("Cladding material: %s density [g/cm^3]: %f \n",Run.Reactor->CladName,Run.Reactor->clad_rho);
  printf("Structural Material: %s density [g/cm^3]: %f frac of core volume: %f\n",Run.Reactor->MatlFront[3]->Name,Run.Reactor->MatlFront[3]->density,Run.Reactor->MatlFront[3]->volpct);
  set_all_actinides(Run.Reactor->T[Run.Reactor->Fission_source_region]);
}
