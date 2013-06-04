/*
 * contrib/decimal/decimal.c
 */
#include "postgres.h"

#include "utils/builtins.h"
#include "utils/formatting.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

#include <stdio.h>
#include <math.h>

static _Decimal32 DatumGetDecimal32(Datum X);
static Datum Decimal32GetDatum(_Decimal32 X);
static _Decimal64 DatumGetDecimal64(Datum X);
static Datum Decimal64GetDatum(_Decimal64 X);

#define PG_GETARG_DECIMAL32(n)	 DatumGetDecimal32(PG_GETARG_DATUM(n))
#define PG_GETARG_DECIMAL64(n)	 DatumGetDecimal64(PG_GETARG_DATUM(n))

#define PG_RETURN_DECIMAL32(x)	 return Decimal32GetDatum(x)
#define PG_RETURN_DECIMAL64(x)	 return Decimal64GetDatum(x)

#define MAXDECIMAL32WIDTH	128
#define MAXDECIMAL64WIDTH	128

/*
 *		====================
 *		FORWARD DECLARATIONS
 *		====================
 */

Datum decimal32_in(PG_FUNCTION_ARGS);
Datum decimal32_out(PG_FUNCTION_ARGS);
Datum decimal64_in(PG_FUNCTION_ARGS);
Datum decimal64_out(PG_FUNCTION_ARGS);

Datum decimal32_div(PG_FUNCTION_ARGS);
Datum decimal32_mul(PG_FUNCTION_ARGS);
Datum decimal32_sum(PG_FUNCTION_ARGS);
Datum decimal32_mi(PG_FUNCTION_ARGS);

Datum decimal64_div(PG_FUNCTION_ARGS);
Datum decimal64_mul(PG_FUNCTION_ARGS);
Datum decimal64_sum(PG_FUNCTION_ARGS);
Datum decimal64_mi(PG_FUNCTION_ARGS);

Datum decimal64_decimal32(PG_FUNCTION_ARGS);
Datum decimal32_decimal64(PG_FUNCTION_ARGS);
Datum float4_decimal32(PG_FUNCTION_ARGS);
Datum float8_decimal64(PG_FUNCTION_ARGS);
Datum decimal64_numeric(PG_FUNCTION_ARGS);
Datum numeric_decimal32(PG_FUNCTION_ARGS);
Datum numeric_decimal64(PG_FUNCTION_ARGS);
Datum int4_decimal32(PG_FUNCTION_ARGS);
Datum int4_decimal64(PG_FUNCTION_ARGS);
Datum int8_decimal32(PG_FUNCTION_ARGS);
Datum int8_decimal64(PG_FUNCTION_ARGS);

Datum decimal32_round(PG_FUNCTION_ARGS);
Datum decimal64_round(PG_FUNCTION_ARGS);


/*
 *		=================
 *		UTILITY FUNCTIONS
 *		=================
 */

static Datum
Decimal32GetDatum(_Decimal32 X)
{
	union
	{
		_Decimal32		value;
		int32			retval;
	}				myunion;

	myunion.value = X;
	return SET_4_BYTES(myunion.retval);
}

static _Decimal32
DatumGetDecimal32(Datum X)
{
	union
	{
		int32		value;
		_Decimal32	retval;
	}			myunion;

	myunion.value = GET_4_BYTES(X);
	return myunion.retval;
}

static Datum
Decimal64GetDatum(_Decimal64 X)
{
#ifdef USE_FLOAT8_BYVAL
	union
	{
		_Decimal64	value;
		int64		retval;
	}			myunion;

	myunion.value = X;
	return SET_*_BYTES(myunion.retval);
#else
	_Decimal64	*retval = (_Decimal64 *) palloc(sizeof(_Decimal64));

	*retval = X;
	return PointerGetDatum(retval);
#endif
}

static _Decimal64
DatumGetDecimal64(Datum X)
{
#ifdef USE_FLOAT8_BYVAL
	union
	{
		int64		value;
		_Decimal	retval;
	}			myunion;

	myunion.value = GET_8_BYTES(X);
	return myunion.retval;
#else
	return *((_Decimal64 *) DatumGetPointer(X));
#endif
}


PG_FUNCTION_INFO_V1(decimal32_in);

Datum
decimal32_in(PG_FUNCTION_ARGS)
{
	char *num = PG_GETARG_CSTRING(0);
	char *endptr;
	double val;
	_Decimal32	result;

	errno = 0;
	val = strtod(num, &endptr);

	if (errno != 0)
		elog(ERROR, "invalid input syntax for type decimal32:\"%s\"", num);

	result = (_Decimal32) val;

	if ((double) result != val)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("decimal32 out of range"),
				 errhint("allows only 7 digits")));

	PG_RETURN_DECIMAL32(result);
}

/*
 * Don't show useles zeroes and more than supported
 * digits.
 */
static void
cleanup_number(char *str, int size)
{
	char *last_nonzero = NULL;
	bool found_decimal_point = false;

	while (size > 0 && *str != '\0')
	{
		if (*str == '.')
		{
			found_decimal_point = true;
			last_nonzero = str - 1;
			str++;
			continue;
		}

		if (found_decimal_point && *str != '0')
			last_nonzero = str;

		str++;
		size--;
	}

	if (last_nonzero != NULL)
		last_nonzero[1] = '\0';
	else if (size == 0 && *str != '\0')
		str[0] = '\0';
}

PG_FUNCTION_INFO_V1(decimal32_out);

Datum
decimal32_out(PG_FUNCTION_ARGS)
{
	_Decimal32 d = PG_GETARG_DECIMAL32(0);
	char *ascii = (char *) palloc(MAXDECIMAL32WIDTH + 1);

	snprintf(ascii, MAXDECIMAL32WIDTH + 1, "%f", (double) d);

	cleanup_number(ascii, 7);

	PG_RETURN_CSTRING(ascii);
}

PG_FUNCTION_INFO_V1(decimal64_in);

Datum
decimal64_in(PG_FUNCTION_ARGS)
{
	char *num = PG_GETARG_CSTRING(0);
	char *endptr;
	long double val;
	_Decimal64	result;

	errno = 0;
	val = strtold(num, &endptr);

	if (errno != 0)
		elog(ERROR, "invalid input syntax for type decimal64:\"%s\"", num);

	result = (_Decimal64) val;

	if ((long double) result != val)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("decimal64 out of range"),
				 errhint("allows only 16 digits")));

	PG_RETURN_DECIMAL64((_Decimal64) val);
}

PG_FUNCTION_INFO_V1(decimal64_out);

Datum
decimal64_out(PG_FUNCTION_ARGS)
{
	_Decimal64 d = PG_GETARG_DECIMAL64(0);
	char *ascii = (char *) palloc(MAXDECIMAL64WIDTH + 1);

	snprintf(ascii, MAXDECIMAL64WIDTH + 1, "%Lf", (long double) d);

	cleanup_number(ascii, 16);

	PG_RETURN_CSTRING(ascii);
}

PG_FUNCTION_INFO_V1(decimal32_div);

Datum
decimal32_div(PG_FUNCTION_ARGS)
{
	_Decimal64 a = (_Decimal64) PG_GETARG_DECIMAL32(0);
	_Decimal64 b = (_Decimal64) PG_GETARG_DECIMAL32(1);

	PG_RETURN_DECIMAL64(a / b);
}

PG_FUNCTION_INFO_V1(decimal32_mul);

Datum
decimal32_mul(PG_FUNCTION_ARGS)
{
	_Decimal64 a = (_Decimal64) PG_GETARG_DECIMAL32(0);
	_Decimal64 b = (_Decimal64) PG_GETARG_DECIMAL32(1);

	PG_RETURN_DECIMAL64(a * b);
}

PG_FUNCTION_INFO_V1(decimal32_sum);

Datum
decimal32_sum(PG_FUNCTION_ARGS)
{
	_Decimal64 a = (_Decimal64) PG_GETARG_DECIMAL32(0);
	_Decimal64 b = (_Decimal64) PG_GETARG_DECIMAL32(1);

	PG_RETURN_DECIMAL64(a + b);
}

PG_FUNCTION_INFO_V1(decimal32_mi);

Datum
decimal32_mi(PG_FUNCTION_ARGS)
{
	_Decimal64 a = (_Decimal64) PG_GETARG_DECIMAL32(0);
	_Decimal64 b = (_Decimal64) PG_GETARG_DECIMAL32(1);

	PG_RETURN_DECIMAL64(a - b);
}

PG_FUNCTION_INFO_V1(decimal64_sum);

Datum
decimal64_sum(PG_FUNCTION_ARGS)
{
	_Decimal64 a = PG_GETARG_DECIMAL64(0);
	_Decimal64 b = PG_GETARG_DECIMAL64(1);
	_Decimal64 result;

	errno = 0;
	result = a + b;

	if (result - a != b)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("decimal64 out of range"),
				 errhint("allows only 16 digits")));

	PG_RETURN_DECIMAL64(result);
}

PG_FUNCTION_INFO_V1(decimal64_div);

Datum
decimal64_div(PG_FUNCTION_ARGS)
{
	_Decimal64 a = PG_GETARG_DECIMAL64(0);
	_Decimal64 b = PG_GETARG_DECIMAL64(1);

	PG_RETURN_DECIMAL64(a / b);
}

PG_FUNCTION_INFO_V1(decimal64_mul);

Datum
decimal64_mul(PG_FUNCTION_ARGS)
{
	_Decimal64 a = PG_GETARG_DECIMAL64(0);
	_Decimal64 b = PG_GETARG_DECIMAL64(1);
	_Decimal64 result;

	result = a * b;
	if (result / a != b)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("decimal64 out of range"),
				 errhint("allows only 16 digits")));

	PG_RETURN_DECIMAL64(a * b);
}

PG_FUNCTION_INFO_V1(decimal64_mi);

Datum
decimal64_mi(PG_FUNCTION_ARGS)
{
	_Decimal64 a = PG_GETARG_DECIMAL64(0);
	_Decimal64 b = PG_GETARG_DECIMAL64(1);

	PG_RETURN_DECIMAL64(a - b);
}

/*
 * Casts
 */
PG_FUNCTION_INFO_V1(decimal64_decimal32);

Datum
decimal64_decimal32(PG_FUNCTION_ARGS)
{
	_Decimal64 a = PG_GETARG_DECIMAL64(0);
	_Decimal32 result;

	result = (_Decimal32) a;
	if ((_Decimal64) result != a)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("decimal32 out of range"),
				 errhint("allows only 7 digits")));

	PG_RETURN_DECIMAL32(result);
}

PG_FUNCTION_INFO_V1(decimal32_decimal64);

Datum
decimal32_decimal64(PG_FUNCTION_ARGS)
{
	_Decimal32 a = PG_GETARG_DECIMAL32(0);

	PG_RETURN_DECIMAL64((_Decimal64) a);
}

PG_FUNCTION_INFO_V1(float4_decimal32);

Datum
float4_decimal32(PG_FUNCTION_ARGS)
{
	float4		num = PG_GETARG_FLOAT4(0);
	_Decimal32	result;

	result = (_Decimal32) num;
	if ((float4) result != num)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("decimal32 out of range"),
				 errhint("allows only 7 digits")));

	PG_RETURN_DECIMAL32(result);
}

PG_FUNCTION_INFO_V1(float8_decimal64);

Datum
float8_decimal64(PG_FUNCTION_ARGS)
{
	float8		num = PG_GETARG_FLOAT8(0);
	_Decimal64	result;

	result = (_Decimal64) num;
	if ((float8) result != num)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("decimal64 out of range"),
				 errhint("allows only 16 digits")));

	PG_RETURN_DECIMAL64(result);
}

PG_FUNCTION_INFO_V1(decimal64_numeric);

Datum
decimal64_numeric(PG_FUNCTION_ARGS)
{
	char		*tmp;
	Datum		result;

	tmp = DatumGetCString(DirectFunctionCall1(decimal64_out,
								    PG_GETARG_DATUM(0)));
	result = DirectFunctionCall3(numeric_in,
						    CStringGetDatum(tmp),
						    Int32GetDatum(0),
						    Int32GetDatum(-1));

	pfree(tmp);

	PG_RETURN_DATUM(result);
}

PG_FUNCTION_INFO_V1(numeric_decimal32);

Datum
numeric_decimal32(PG_FUNCTION_ARGS)
{
	char		*tmp;
	Datum		result;

	tmp = DatumGetCString(DirectFunctionCall1(numeric_out,
								PG_GETARG_DATUM(0)));
	result = DirectFunctionCall1(decimal32_in, CStringGetDatum(tmp));

	pfree(tmp);

	PG_RETURN_DATUM(result);
}

PG_FUNCTION_INFO_V1(numeric_decimal64);

Datum
numeric_decimal64(PG_FUNCTION_ARGS)
{
	char		*tmp;
	Datum		result;

	tmp = DatumGetCString(DirectFunctionCall1(numeric_out,
								PG_GETARG_DATUM(0)));

	result = DirectFunctionCall1(decimal64_in, CStringGetDatum(tmp));

	pfree(tmp);

	PG_RETURN_DATUM(result);
}

PG_FUNCTION_INFO_V1(int4_decimal32);

Datum
int4_decimal32(PG_FUNCTION_ARGS)
{
	int32 a = PG_GETARG_INT32(0);
	_Decimal32 result;

	result = (_Decimal32) a;
	if ((int32) result != a)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("decimal32 out of range"),
				 errhint("allows only 7 digits")));

	PG_RETURN_DECIMAL32(result);
}

PG_FUNCTION_INFO_V1(int4_decimal64);

Datum
int4_decimal64(PG_FUNCTION_ARGS)
{
	int32 a = PG_GETARG_INT32(0);
	_Decimal64 result;

	result = (_Decimal64) a;
	if ((int32) result != a)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("decimal64 out of range"),
				 errhint("allows only 16 digits")));

	PG_RETURN_DECIMAL64(result);
}

PG_FUNCTION_INFO_V1(int8_decimal32);

Datum
int8_decimal32(PG_FUNCTION_ARGS)
{
	int64 a = PG_GETARG_INT64(0);
	_Decimal32 result;

	result = (_Decimal32) a;
	if ((int64) result != a)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("decimal32 out of range"),
				 errhint("allows only 7 digits")));

	PG_RETURN_DECIMAL32(result);
}

PG_FUNCTION_INFO_V1(int8_decimal64);

Datum
int8_decimal64(PG_FUNCTION_ARGS)
{
	int64 a = PG_GETARG_INT64(0);
	_Decimal64 result;

	result = (_Decimal64) a;
	if ((int64) result != a)
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("decimal64 out of range"),
				 errhint("allows only 16 digits")));

	PG_RETURN_DECIMAL64(result);
}

PG_FUNCTION_INFO_V1(decimal32_round);

Datum
decimal32_round(PG_FUNCTION_ARGS)
{
	_Decimal32 d = PG_GETARG_DECIMAL32(0);
	int32 scale = PG_GETARG_INT32(1);
	double mu;


	mu = pow(10.0, (double) scale);
	PG_RETURN_DECIMAL32((_Decimal32)(round((double) d * mu) / mu));
}


PG_FUNCTION_INFO_V1(decimal64_round);

Datum
decimal64_round(PG_FUNCTION_ARGS)
{
	_Decimal64 d = PG_GETARG_DECIMAL64(0);
	int32 scale = PG_GETARG_INT32(1);
	double mu;

	if (scale < 0)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("scale should be positive number or zero")));

	mu = pow(10.0, (double) scale);

	PG_RETURN_DECIMAL64((_Decimal64)(round((double) d * mu) / mu));
}
