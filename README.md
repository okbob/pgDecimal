pgDecimal
=========

PostgreSQL types based on _Decimal32 and _Decimal64 types.

I wrote this library due performance comparation with PostgreSQL's Numeric type. I would to know how much
big impact has fact, so Numeric is varlena type. This initial implementation is based on basic _Decimal
support, that is not complete (and check of overflow is slower, than could be with better library). This library 
contains some basic arithmetic operators and basic casts.

<pre>
postgres=# CREATE EXTENSION decimal;
CREATE EXTENSION
postgres=# SELECT 3.14::decimal32;
 decimal32 
───────────
 3.14
(1 row)

postgres=# SELECT 3.14::decimal64;
 decimal64 
───────────
 3.14
(1 row)
</pre>
