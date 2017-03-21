#ifndef DEFINE_H_
#define DEFINE_H_

struct DATA_XYZ
{
	short X;
	short Y;
	short Z;
};
struct DATA_XYZ_F
{
	float X;
	float Y;
	float Z;
};
struct DATA_16
{
	int THR;
	int FB;
	int LR;
	int TURN;
};
struct PID
{
	float P;
	float I;
	float D;
	float IMAX;
	float SUM_ERROR_R;
	float SUM_ERROR_P;
};	

#endif /*DEFINE_H_*/
