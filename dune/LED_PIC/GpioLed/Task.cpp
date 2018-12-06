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
// Author: Hugo Antunes                                                     *
//***************************************************************************

// DUNE headers.
#include <DUNE/DUNE.hpp>
#include <DUNE/Hardware/GPIO.hpp>

namespace Tutorials
{
  //! Insert short task description here.
  //!
  //! Insert explanation on task behaviour here.
  //! @author Hugo Antunes
  namespace GpioLed
  {
    using DUNE_NAMESPACES;

    struct Task: public DUNE::Tasks::Task
    {
	    Hardware::GPIO* m_gpio;
      //! Constructor.
      //! @param[in] name task name.
      //! @param[in] ctx context.
      Task(const std::string& name, Tasks::Context& ctx):
        DUNE::Tasks::Task(name, ctx),
	m_gpio(NULL)
      {
	      bind<IMC::SetLedBrightness>(this);
      }

      void consume (const IMC::SetLedBrightness* msg) {
		(void)msg;
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
	      m_gpio = new Hardware::GPIO(11);
	      m_gpio->setDirection(Hardware::GPIO::GPIO_DIR_OUTPUT);
	      m_gpio->setValue(0);
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
	      Memory::clear(m_gpio);
      }

      //! Main loop.
      void
      onMain(void)
      {
	      int lat=0, longit = 0;
	      std::string save, path;
	      path = "/home/pi/Pics";
        while (!stopping())
        {
		lat+=1;
		longit+=1;
		consumeMessages();

		if (m_gpio == NULL)
			return;

		m_gpio->setValue(1);
		Delay::wait(3.0);
		m_gpio->setValue(0);
		Delay::wait(3.0);
		save = std::to_string(lat) + std::to_string(longit);
		save = "raspistill -n -o " + path + "/" + save + ".jpg";
		system(save.c_str());
		Delay::wait(5.0);
        }
      }
    };
  }
}

DUNE_TASK
