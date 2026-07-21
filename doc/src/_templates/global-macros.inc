{% macro cppsnippet(tag) -%}
.. literalinclude:: Snippets.cc
    :language: cpp
    :start-after: //!{{ tag }}
    :end-before: //!End
{%- endmacro %}

{% macro video(url) -%}
.. raw:: html

   <p><video autoplay loop controls onclick="this.paused ? this.play() : this.pause();" src="{{ url }}"></video></p>

{%- endmacro %}

{% macro video_noloop(url) -%}
.. raw:: html

   <p><video autoplay controls onclick="this.paused ? this.play() : this.pause();" src="{{ url }}"></video></p>

{%- endmacro %}
