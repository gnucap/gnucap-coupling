/*                              -*- C++ -*-
 * Copyright (C) 2013-2017 Felix Salfelder
 * Author: Felix Salfelder
 *
 * This file is part of "gnucap-coupling".
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * FILE bm plugin
 */

#include "e_elemnt.h"
#include "globals.h"
#include "u_lang.h"
#include "l_denoise.h"
#include "bm.h"
#include "m_wave.h" // forked.

#include <boost/assign.hpp>
#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
using std::map;
using std::string;
/*--------------------------------------------------------------------------*/
static const double _default_iv=0;
static const double _default_delay=0;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class EVAL_BM_FILE : public EVAL_BM_ACTION_BASE {
private:
  PARAMETER<double> _delay;
  PARAMETER<double> _iv;
  PARAMETER<double> _filename; // HACK
  //std::string _filename;
  void set_param_by_name(string Name, string Value);

  static map<string, PARA_BASE EVAL_BM_FILE::*> _param_dict;
  explicit	EVAL_BM_FILE(const EVAL_BM_FILE& p);
public:
  explicit      EVAL_BM_FILE(int c=0);
		~EVAL_BM_FILE()		{}
private: // override vitrual
  bool		operator==(const COMMON_COMPONENT&)const;
  COMMON_COMPONENT* clone()const	{return new EVAL_BM_FILE(*this);}
  void		print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const;

  void		precalc_first(const CARD_LIST*);
  void		precalc_last(const CARD_LIST*);
  void		tr_eval(ELEMENT*)const;
  TIME_PAIR	tr_review(COMPONENT*)const;
  void tr_accept(COMPONENT*)const;
  std::string	name()const		{return "file";}
  bool		ac_too()const		{return false;}
  bool		parse_numlist(CS&);
  bool		parse_params_obsolete_callback(CS&);
private: // i/o
  void read_pair() const;
  void read_wave(double until) const;
private:
  //mutable std::fstream _file;
  int _fd;
  FILE* _file;
  mutable WAVE _wave;
public:
  void expand(const COMPONENT* d);
private: // common in all commons.
};
/*--------------------------------------------------------------------------*/
EVAL_BM_FILE::EVAL_BM_FILE(int c)
  :EVAL_BM_ACTION_BASE(c),
	_delay(0), _iv(0), _filename(),
   _fd(0),
   _file(NULL)
{ untested();
}
/*--------------------------------------------------------------------------*/
EVAL_BM_FILE::EVAL_BM_FILE(const EVAL_BM_FILE& p)
  :EVAL_BM_ACTION_BASE(p),
	_delay(p._delay),
	_iv(p._iv),
	_filename(p._filename), _fd(p._fd)
{ untested();
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_FILE::operator==(const COMMON_COMPONENT& x)const
{ untested();
	return false; // buffer in COMMON...
	const EVAL_BM_FILE* p = dynamic_cast<const EVAL_BM_FILE*>(&x);
	bool rv = p;
	return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_FILE::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{ untested();
	incomplete();
  assert(lang);
  o << name();
  EVAL_BM_ACTION_BASE::print_common_obsolete_callback(o, lang);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_FILE::precalc_first(const CARD_LIST* Scope)
{ untested();
  assert(Scope);
  EVAL_BM_ACTION_BASE::precalc_first(Scope);
  _iv.e_val(_default_iv, Scope);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_FILE::precalc_last(const CARD_LIST* Scope)
{ untested();
  _delay.e_val(_default_delay, Scope);

//   trace1("filename?", _filename.string());
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_FILE::expand(const COMPONENT* d)
{ untested();
	trace1("expand", d->long_label());

//	std::ios_base::openmode mode;
   int fdmode = 0; // O_ASYNC;
   const char* mode;

	// HACK HACK HACK
	std::string filename;
	if(_filename.has_hard_value()){ untested();
		filename = _filename.string();
	}else{ untested();
		filename = d->long_label();
	}

	if (d->is_source()) { untested();
		// mode = std::ios_base::in;
		fdmode &= O_RDONLY | O_NONBLOCK;
		_fd = open(filename.c_str(), O_RDONLY | O_NONBLOCK);
		_file = fdopen(_fd, "r");
		assert(_file);
	}else{ untested();
		// mode = std::ios_base::out;
		trace3("opening", filename, _fd, _file);
		_fd = open(filename.c_str(), O_CREAT|O_WRONLY);
		assert(_fd);
		untested();
		trace3("df opening", filename, _fd, _file);
		_file = fdopen(_fd, "w");
		untested();
	}

#if 0
	try{ untested();
		trace2("open", filename, mode);
		_file.open(filename, mode);
	}catch(...){ untested();
		incomplete();
		exit(3);
	}
#endif

	assert(_file);

	trace3("opened", filename, _fd, _file);

	if (!d->is_source()) { untested();
	}else if(_iv.has_hard_value()){ untested();
		_wave.push(0., _iv);
	}else if(_wave.begin()==_wave.end()){ untested();
		read_pair();
	}else{ untested();
		// just clear? unreachable?
		incomplete();
	}
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_FILE::read_pair() const
{ untested();
	double time;
	double val;
#if 0
	_file >> time;
	_file >> val;

	if(_file.eof()){ untested();
		incomplete();
	}else{ untested();
	}
#else
	int ret=fscanf(_file, "%lf %lf\n", &time, &val);
	if(ret==EOF){ untested();
		incomplete();
		error(bDANGER, "EOF\n");
	}else if(ret!=2){ untested();
		trace1("fscanf", ret);
	}
	trace2("read_pair", time, val);
#endif

	assert(_wave.back().first < time || time==0);
	_wave.push(time, val);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_FILE::read_wave(double until) const
{ untested();
	trace1("read_wave", until);
	assert(_wave.end()!=_wave.begin());
	while(_wave.back().first < until){ untested();
		read_pair();
	}
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_FILE::tr_eval(ELEMENT* d)const
{ untested();
	trace1("tr_eval", _delay);
	double ev = 0;

	if (d->is_source()) { untested();
		read_wave(d->_time[0]-_delay);
//		ev = _wave.v_out(_time);
		auto fp = _wave.v_out(d->_time[0]-_delay);

		tr_finish_tdv(d, fp.f0);
	}else{ untested();
	}
//	tr_finish_tdv(d, ev);
}
/*--------------------------------------------------------------------------*/
TIME_PAIR EVAL_BM_FILE::tr_review(COMPONENT* d)const
{ untested();
  d->q_accept();
#ifdef HAVE_DTIME
  return d->_dt_by;
#else
  return d->_time_by;
#endif
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_FILE::tr_accept(COMPONENT*d)const
{ untested();

	incomplete();
	ELEMENT* e = prechecked_cast<ELEMENT*>(d);
	assert(e);

	if (!d->is_source()) { untested();
#if 0
		_file << e->_time[0] << " " << e->tr_input() << "\n";
		_file.flush();
#else
		fprintf(_file, "%f %f\n", e->_time[0], e->tr_input());
		fflush(_file);
		fprintf(stderr, "%f %f\n", e->_time[0], e->tr_input());
#endif
	}else{ untested();
		// pop
	}
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_FILE::parse_numlist(CS& cmd)
{ untested();
	incomplete();
  unsigned start = cmd.cursor();
  unsigned here = cmd.cursor();
#if 0
  for (PARAMETER<double>* i = &_direction;  i < &_end;  ++i) { untested();
    PARAMETER<double> val(NOT_VALID);
    cmd >> val;
    if (cmd.stuck(&here)) { untested();
      break;
    }else{ untested();
      *i = val;
    }
  }
#endif
  return cmd.gotit(start);
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_FILE::parse_params_obsolete_callback(CS& cmd)
{ incomplete();
  return ONE_OF
    || EVAL_BM_ACTION_BASE::parse_params_obsolete_callback(cmd)
    ;
}
/*--------------------------------------------------------------------------*/
map<string, PARA_BASE EVAL_BM_FILE::*> EVAL_BM_FILE::_param_dict =
  boost::assign::map_list_of
    ("delay",     (PARA_BASE EVAL_BM_FILE::*) &EVAL_BM_FILE::_delay)
    ("iv",        (PARA_BASE EVAL_BM_FILE::*) &EVAL_BM_FILE::_iv)
    ("filename",  (PARA_BASE EVAL_BM_FILE::*) &EVAL_BM_FILE::_filename);
/*--------------------------------------------------------------------------*/
void EVAL_BM_FILE::set_param_by_name(std::string Name, std::string Value)
{ untested();
  PARA_BASE EVAL_BM_FILE::* x = (_param_dict[Name]);
  if (x) { untested();
    PARA_BASE* p = &(this->*x);
    *p = Value;
  }else{ untested();
    EVAL_BM_ACTION_BASE::set_param_by_name(Name, Value);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
EVAL_BM_FILE p1(CC_STATIC);
DISPATCHER<COMMON_COMPONENT>::INSTALL d1(&bm_dispatcher, "file0", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
