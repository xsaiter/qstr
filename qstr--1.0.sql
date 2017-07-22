-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "create extension qstr" to load this file. \quit

create function q_str_least_to_palindrome (text) returns int4
as '$libdir/libqstr'
language C immutable strict;

create function q_str_levenshtein_distance (text, text) returns int4
as '$libdir/libqstr'
language C immutable strict;

create function q_str_is_palindrome (text) returns boolean
as '$libdir/libqstr'
language C immutable strict;