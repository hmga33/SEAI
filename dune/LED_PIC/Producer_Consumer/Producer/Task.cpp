//***************************************************************************
// Copyright 2007-2017 Universidade do Porto - Faculdade de Engenharia      *
// Laboratório de Sistemas e Tecnologia Subaquática (LSTS)                  *
//***************************************************************************
// This file is part of DUNE: Unified Navigation Environment.               *
//                                                                          *
// Commercial Licence Usage                                                 *
// Licencees holding valid commercial DUNE licences may use this file in    *
// accordance with the commercial licence agreement provided with the       *
// Software or, alternatively, in accordance with the terms contained in a  *
// written agreement between you and Faculdade de Engenharia da             *
// Universidade do Porto. For licensing terms, conditions, and further      *
// information contact lsts@fe.up.pt.                                       *
//                                                                          *
// Modified European Union Public Licence - EUPL v.1.1 Usage                *
// Alternatively, this file may be used under the terms of the Modified     *
// EUPL, Version 1.1 only (the "Licence"), appearing in the file LICENCE.md *
// included in the packaging of this file. You may not use this work        *
// except in compliance with the Licence. Unless required by applicable     *
// law or agreed to in writing, software distributed under the Licence is   *
// distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF     *
// ANY KIND, either express or implied. See the Licence for the specific    *
// language governing permissions and limitations at                        *
// https://github.com/LSTS/dune/blob/master/LICENCE.md and                  *
// http://ec.europa.eu/idabc/eupl.html.                                     *
//***************************************************************************
// Author: Ricardo Martins                                                  *
//***************************************************************************

// DUNE headers.
#include <DUNE/DUNE.hpp>

namespace Workshop
{
  //! Insert short task description here.
  //!
  //! Insert explanation on task behaviour here.
  //! @author Ricardo Martins
  namespace Producer
  {
    using DUNE_NAMESPACES;

    struct Arguments {
	    std::string prng_type;
	    int prng_seed;
	    float mean_value;
	    double std_dev;
    };

    struct Task: public DUNE::Tasks::Periodic
    {
	Random::Generator* m_prng;
	Arguments m_args;
      //! Constructor.
      //! @param[in] name task name.
      //! @param[in] ctx context.
      Task(const std::string& name, Tasks::Context& ctx):
        DUNE::Tasks::Periodic(name, ctx),
	m_prng(NULL)
      {
	      param("Standard Deviation", m_args.std_dev)
		      .units(Units::Meter)
		      .defaultValue("0.1");

	      param("PRNG Type", m_args.prng_type)
		      .defaultValue(Random::Factory::c_default);

	      param("PRNG Seed", m_args.prng_seed)
		      .defaultValue("-1");

	      param("Mean Value", m_args.mean_value)
		      .defaultValue("25.0")
		      .units(Units::DegreeCelsius)
		      .description("Mean Temperature Value");
      }

      //! Update internal state with new parameter values.
      void
      onUpdateParameters(void)
      {
      }

      //! Reserve entity identifiers.
      void
      onEntityReservation(void)
      {
      }

      //! Resolve entity names.
      void
      onEntityResolution(void)
      {
      }

      //! Acquire resources.
      void
      onResourceAcquisition(void)
      {
	      m_prng = Random::Factory::create(m_args.prng_type, m_args.prng_seed);
      }
      //! Initialize resources.
      void
      onResourceInitialization(void)
      {
      }

      //! Release resources.
      void
      onResourceRelease(void)
      {
	      Memory::clear(m_prng);
      }

      //! Main loop.
      void
      task(void)
      {
        IMC::Temperature temperature;
	temperature.value = m_args.mean_value
				+ m_prng->gaussian()
				* m_args.std_dev;
	dispatch(temperature);
      }
    };
  }
}

DUNE_TASK
