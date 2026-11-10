Simulation Manual
=================

.. only:: html

   .. raw:: html

      <script>
        // Before the chapters had pages of their own, their sections were on this page.
        if (location.hash) {
          fetch('anchors.json').then(function (response) { return response.json(); }).then(function (pages) {
            var page = pages[location.hash.slice(1)];
            if (page) location.replace(page + '.html' + location.hash);
          });
        }
      </script>

   :doc:`Single page version <sp/index>` | `PDF version <../pdf/SimulationManual.pdf>`__

   Table of Contents

.. toctree::
   :numbered: 3
   :maxdepth: 2
   :titlesonly:

   ch-introduction
   ch-overview
   ch-ned-lang
   ch-simple-modules
   ch-messages
   ch-message-definitions
   ch-sim-lib
   ch-graphics
   ch-build-sim-progs
   ch-config-sim
   ch-run-sim
   ch-ana-sim
   ch-eventlog
   ch-neddoc
   ch-testing
   ch-parallel-exec
   ch-plugin-exts
   ch-embedding
   appendix-ned-ref
   appendix-ned-grammar
   appendix-ned-dtd
   appendix-ned-functions
   appendix-msg-grammar
   appendix-msg-properties
   appendix-display-strings
   appendix-figure-definitions
   appendix-config-options
   appendix-result-file-formats
   appendix-eventlog-file-format
   appendix-chart-api
