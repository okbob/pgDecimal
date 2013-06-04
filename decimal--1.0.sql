/* contrib/citext/citext--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION citext" to load this file. \quit

--
--  PostgreSQL code for CITEXT.
--
-- Most I/O functions, and a few others, piggyback on the "text" type
-- functions via the implicit cast to text.
--

--
-- Shell type to keep things a bit quieter.
--

CREATE TYPE decimal32;

--
--  Input and output functions.
--
CREATE FUNCTION decimal32_in(cstring)
RETURNS decimal32
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION decimal32_out(decimal32)
RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE decimal32 (
    INPUT          = decimal32_in,
    OUTPUT         = decimal32_out,
    LIKE = pg_catalog.int4
);

CREATE TYPE decimal64;

CREATE FUNCTION decimal64_in(cstring)
RETURNS decimal64
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION decimal64_out(decimal64)
RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE decimal64 (
    INPUT          = decimal64_in,
    OUTPUT         = decimal64_out,
    LIKE = pg_catalog.int8
);


CREATE FUNCTION decimal32_div(decimal32, decimal32)
RETURNS decimal64
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION decimal32_mul(decimal32, decimal32)
RETURNS decimal64
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION decimal32_sum(decimal32, decimal32)
RETURNS decimal64
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION decimal32_mi(decimal32, decimal32)
RETURNS decimal64
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION decimal64_sum(decimal64, decimal64)
RETURNS decimal64
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION decimal64_div(decimal64, decimal64)
RETURNS decimal64
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION decimal64_mul(decimal64, decimal64)
RETURNS decimal64
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION decimal64_mi(decimal64, decimal64)
RETURNS decimal64
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE AGGREGATE sum(decimal64)
(
	sfunc = decimal64_sum,
	stype = decimal64
);

CREATE OPERATOR / (
  PROCEDURE = decimal32_div,
  LEFTARG = decimal32, RIGHTARG = decimal32
);

CREATE OPERATOR * (
  PROCEDURE = decimal32_mul,
  LEFTARG = decimal32, RIGHTARG = decimal32
);

CREATE OPERATOR + (
  PROCEDURE = decimal32_sum,
  LEFTARG = decimal32, RIGHTARG = decimal32
);

CREATE OPERATOR - (
  PROCEDURE = decimal32_mi,
  LEFTARG = decimal32, RIGHTARG = decimal32
);

CREATE OPERATOR / (
  PROCEDURE = decimal64_div,
  LEFTARG = decimal64, RIGHTARG = decimal64
);

CREATE OPERATOR * (
  PROCEDURE = decimal64_mul,
  LEFTARG = decimal64, RIGHTARG = decimal64
);

CREATE OPERATOR + (
  PROCEDURE = decimal64_sum,
  LEFTARG = decimal64, RIGHTARG = decimal64
);

CREATE OPERATOR - (
  PROCEDURE = decimal64_mi,
  LEFTARG = decimal64, RIGHTARG = decimal64
);

/*
 * Casts functions
 */
CREATE FUNCTION decimal64_decimal32(decimal64)
RETURNS decimal32
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION decimal32_decimal64(decimal32)
RETURNS decimal64
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION float4_decimal32(float4)
RETURNS decimal32
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION float8_decimal64(float8)
RETURNS decimal64
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION numeric_decimal32(numeric)
RETURNS decimal32
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION numeric_decimal64(numeric)
RETURNS decimal64
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION decimal64_numeric(decimal64)
RETURNS numeric
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION int4_decimal32(int4)
RETURNS decimal32
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION int4_decimal64(int4)
RETURNS decimal64
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION int8_decimal32(int8)
RETURNS decimal32
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION int8_decimal64(int8)
RETURNS decimal64
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE CAST(decimal64 AS decimal32)
WITH FUNCTION decimal64_decimal32(decimal64);

CREATE CAST(decimal32 AS decimal64)
WITH FUNCTION decimal32_decimal64(decimal32)
AS IMPLICIT;

CREATE CAST(float4 AS decimal32)
WITH FUNCTION float4_decimal32(float4)
AS IMPLICIT;

CREATE CAST(float8 AS decimal64)
WITH FUNCTION float8_decimal64(float8)
AS IMPLICIT;

CREATE CAST(numeric AS decimal32)
WITH FUNCTION numeric_decimal32(numeric)
AS IMPLICIT;

CREATE CAST(numeric AS decimal64)
WITH FUNCTION numeric_decimal64(numeric)
AS IMPLICIT;

CREATE CAST(decimal64 AS numeric)
WITH FUNCTION decimal64_numeric(decimal64)
AS IMPLICIT;

CREATE CAST(int4 AS decimal32)
WITH FUNCTION int4_decimal32(int4)
AS IMPLICIT;

CREATE CAST(int4 AS decimal64)
WITH FUNCTION int4_decimal64(int4)
AS IMPLICIT;

CREATE CAST(int8 AS decimal32)
WITH FUNCTION int8_decimal32(int8)
AS IMPLICIT;

CREATE CAST(int8 AS decimal64)
WITH FUNCTION int8_decimal64(int8)
AS IMPLICIT;

/*
 * Other functions
 */
CREATE FUNCTION round(decimal32, int)
RETURNS decimal32
AS 'MODULE_PATHNAME','decimal32_round'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION round(decimal64, int)
RETURNS decimal64
AS 'MODULE_PATHNAME','decimal64_round'
LANGUAGE C IMMUTABLE STRICT;


